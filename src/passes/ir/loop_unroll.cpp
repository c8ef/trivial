#include "passes/ir/loop_unroll.hpp"

#include <cassert>

#include "passes/ir/CFG.hpp"
#include "structure/op.hpp"

static void clone_inst(Inst* x, BasicBlock* bb,
                       std::unordered_map<Value*, Value*>& map) {
  auto get = [&map](const Use& u) {
    Value* v = u.value;
    auto it = map.find(v);
    return it != map.end() ? it->second : v;
  };
  Inst* res;
  if (auto y = dyn_cast<BinaryInst>(x)) {
    res = new BinaryInst(y->tag, get(y->lhs), get(y->rhs), bb);
  } else if (auto y = dyn_cast<GetElementPtrInst>(x)) {
    res = new GetElementPtrInst(y->lhs_sym, get(y->arr), get(y->index),
                                y->multiplier, bb);
  } else if (auto y = dyn_cast<LoadInst>(x)) {
    res = new LoadInst(y->lhs_sym, get(y->arr), get(y->index),
                       bb);  // 不维护内存依赖
  } else if (auto y = dyn_cast<StoreInst>(x)) {
    res =
        new StoreInst(y->lhs_sym, get(y->arr), get(y->data), get(y->index), bb);
  } else {
    // 不可能是 Branch, Jump, resurn, CallInst, Alloca, Phi, MemOp, MemPhi
    UNREACHABLE();
  }
  map.insert_or_assign(x, res);
}

// 这个 pass 不能处理 memdep 信息，需要保证调用它时没有 memdep 信息
void loop_unroll(IrFunc* f) {
  std::vector<Loop*> deepest = ComputeLoopInfo(f).DeepestLoops();
  for (Loop* l : deepest) {
    // 只考虑这样的循环，前端生成这样的 while 循环如果 body 内没有跳转，会被
    // BasicBlockOpt 优化成这样 bb_cond:
    //   ...
    //   if (i(0) < n) br bb_body else br bb_end
    //   或者 if (const 非 0) br bb_body else br bb_end
    //   或者 br bb_body
    // bb_body: ; preds = [bb_cond, bb_body]
    //   i = phi [i(0), bb_cond] [i(x), bb_body]
    //   ...
    //   i(1) = i + C1
    //   ...
    //   i(x) = i(x-1) + Cx
    //   ...
    //   if (i(x) < n) br bb_body else br bb_end // 注意这里是 i(x)，不是 i
    // bb_end:; preds = [bb_cond, bb_body]
    //   ...
    // 其中 i 由循环中的一个 phi 定值，且来源仅有两个：初值 i0 和 i+常数，n
    // 在循环外定值 大小关系可以是除了==, !=外的四个
    if (l->bbs.size() != 1) continue;
    BasicBlock* bb_body = l->bbs[0];
    if (bb_body->pred.size() != 2) continue;
    u32 idx_in_body = bb_body == bb_body->pred[1];
    auto br = dyn_cast<BranchInst>(bb_body->instructions.tail);
    if (!br || br->left != bb_body || br->cond.value->tag < Value::Tag::Lt ||
        br->cond.value->tag > Value::Tag::Gt)
      continue;
    auto cond = static_cast<BinaryInst*>(br->cond.value);
    BasicBlock* bb_cond = bb_body->pred[!idx_in_body];
    BasicBlock* bb_end = br->right;
    auto br0 = dyn_cast<BranchInst>(bb_cond->instructions.tail);
    if (!br0) {
      auto jump = dyn_cast<JumpInst>(bb_cond->instructions.tail);
      if (!jump || jump->next != bb_body || bb_end->pred.size() != 1) continue;
      assert(!isa<PhiInst>(bb_end->instructions.head));
      bb_cond->instructions.Remove(jump);
      delete jump;
      br0 = new BranchInst(ConstValue::Get(1), bb_body, bb_end, bb_cond);
      bb_end->pred.push_back(bb_cond);
      for (Inst* i = bb_body->instructions.head; i; i = i->next) {
        PhiInst* phi = nullptr;
        for (Use* u = i->uses.head; u;) {
          Use* next = u->next;
          if (u->user->bb != bb_body && u->user != phi) {
            if (phi == nullptr) {
              phi = new PhiInst(bb_end);
              phi->incoming_values[0].Set(i);
              phi->incoming_values[1].Set(new UndefValue);
            }
            u->Set(phi);
          }
          u = next;
        }
      }
    } else if (br0->left != bb_body || br0->right != bb_end ||
               bb_end->pred.size() != 2)
      continue;
    u32 idx_in_end = bb_body == bb_end->pred[1];
    BinaryInst* cond0;  // cond0 可能是 nullptr，此时 bb_cond 中以 if (const 非
                        // 0) br bb_body else br bb_end 结尾
    // 如果 bb_cond 是无条件跳转到 bb_body 的 (包括 cond0 本身是 const
    // 或者上面处理的直接 jump)，这意味着循环体至少执行一次 但是我们 loop unroll
    // 后就不能无条件跳转了，因为此时循环会至少执行两次，所以需要把 cond0 改成
    // BinaryInst 并且也要修改它的 n(在下面处理)
    {
      Value* v = br0->cond.value;
      if ((v->tag < Value::Tag::Lt || v->tag > Value::Tag::Gt) &&
          (v->tag != Value::Tag::Const ||
           static_cast<ConstValue*>(v)->imm == 0))
        continue;
      cond0 = dyn_cast<BinaryInst>(v);
    }

    int step = 0;
    int cond0_i0 = -1,
        cond_ix = -1;  // 结果为0/1时继续处理，表示i0/ix在cond0/cond的lhs还是rhs
    Value* i0 = nullptr;
    PhiInst* phi_ix = nullptr;
    for (int pos = 0; cond0_i0 == -1 && pos < 2;
         ++pos) {  // 考虑 i < n 和 n > i 两种写法
      step = 0, cond_ix = pos;
      Value *ix = (&cond->lhs)[cond_ix].value,
            *n = (&cond->lhs)[!cond_ix].value, *i = ix;
      while (true) {
        if (auto x = dyn_cast<BinaryInst>(i)) {
          // 这里只考虑 Add，因为 gvn_gcm pass 会把减常数变成加相反数
          if (auto r = dyn_cast<ConstValue>(x->rhs.value);
              r && x->tag == Value::Tag::Add) {
            step += r->imm;
            i = x->lhs.value;
          } else
            break;
        } else if (auto x = dyn_cast<PhiInst>(i)) {
          if (x->bb != bb_body) break;
          assert(x->incoming_values.size() == 2);
          i0 = x->incoming_values[!idx_in_body].value;
          phi_ix = x;
          if (x->incoming_values[idx_in_body].value != ix) break;
          if (cond0) {
            // 两个 cond 的 n 必须是同一个，这可以保证 bb_body 中的 cond 的 n
            // 不是循环中定义的
            if (cond->tag == cond0->tag && i0 == cond0->lhs.value &&
                n == cond0->rhs.value) {
              cond0_i0 = 0;
              break;
            } else if (op::isrev((op::Op)cond->tag, (op::Op)cond0->tag) &&
                       i0 == cond0->rhs.value && n == cond0->lhs.value) {
              cond0_i0 = 1;
              break;
            } else
              break;
          } else {
            // 如果 cond0 是 ConstValue，则还是需要检查 n 的定义位置，并且把
            // cond0 改回 BinaryInst
            auto n1 = dyn_cast<Inst>(n);
            if (!n1 || n1->bb != bb_body) {
              // cond0 是 ConstValue 一般是 i0 和 n
              // 都是常数，不过为了保险还是检查一下
              if (auto i0c = dyn_cast<ConstValue>(i0),
                  nc = dyn_cast<ConstValue>(n);
                  i0c && nc &&
                  op::Eval((op::Op)cond->tag, cond_ix == 0 ? i0c->imm : nc->imm,
                           cond_ix == 0 ? nc->imm : i0c->imm)) {
                cond0_i0 = cond_ix;
                cond0 = new BinaryInst(cond->tag, cond_ix == 0 ? i0c : nc,
                                       cond_ix == 0 ? nc : i0c, br0);
              } else
                break;
              break;
            } else
              break;
          }
        } else
          break;
      }
    }
    if (cond0_i0 == -1 || step == 0) continue;

    bool inst_ok = true;
    int inst_cnt = 0;
    for (Inst* i = bb_body->instructions.head; inst_ok && i; i = i->next) {
      if (isa<PhiInst>(i) || isa<BranchInst>(i)) continue;
      // 包含 call 的循环没有什么展开的必要
      // 目前不考虑有局部数组的情形，memdep 应该不能处理多个局部数组对应同一个
      // Decl
      else if (isa<CallInst>(i) || isa<AllocaInst>(i) || ++inst_cnt >= 16)
        inst_ok = false;
    }
    if (!inst_ok) continue;

    DEBUG("Performing loop unroll");
    // 验证结束，循环展开，目前为了实现简单仅展开 2 次，如果实现正确的话运行 n
    // 次就可以展开 2^n 次
    std::unordered_map<Value*, Value*> map;
    auto get = [&map](Value* v) {
      auto it = map.find(v);
      return it != map.end() ? it->second : v;
    };
    Inst* first_non_phi = nullptr;
    for (Inst* i = bb_body->instructions.head;; i = i->next) {
      if (auto x = dyn_cast<PhiInst>(i)) {
        map.insert({x, x->incoming_values[idx_in_body].value});
      } else {
        first_non_phi = i;
        break;
      }
    }

    Value* old_n = (&cond->lhs)[!cond_ix].value;
    bb_body->instructions.Remove(
        br);  // 后面需要往 bb_body 的最后
              // insert，所以先把跳转指令去掉，等下再加回来
    Inst* orig_last =
        bb_body->instructions
            .tail;  // 不可能为 null，因为 bb_body 中至少存在一条计算 i1 的指令
    assert(orig_last != nullptr);

    // 先做一个特判，如果可能的话完全展开这个循环
    if (auto i0c = dyn_cast<ConstValue>(i0),
        old_nc = dyn_cast<ConstValue>(old_n);
        i0c && old_nc) {
      i32 beg = i0c->imm, end = old_nc->imm,
          times = (end - beg) / step;  // 这不是精确的次数，可能会差一次
      if (times <= 0 || times > 32) goto normal_unroll;
      beg += step;
      while (op::Eval((op::Op)cond->tag, cond_ix == 0 ? beg : end,
                      cond_ix == 0 ? end : beg)) {
        for (Inst* i = first_non_phi;; i = i->next) {
          clone_inst(i, bb_body, map);
          if (i == orig_last) break;
        }
        for (Inst* i = bb_body->instructions.head;; i = i->next) {
          if (auto x = dyn_cast<PhiInst>(i)) {
            map.find(x)->second = get(x->incoming_values[idx_in_body].value);
          } else
            break;
        }
        beg += step;
      }
      delete br;
      bb_body->pred.erase(bb_body->pred.begin() + idx_in_body);
      new JumpInst(bb_end, bb_body);
      for (Inst* i = bb_end->instructions.head;; i = i->next) {
        if (auto x = dyn_cast<PhiInst>(i)) {
          x->incoming_values[idx_in_end].Set(
              get(x->incoming_values[idx_in_end].value));
        } else
          break;
      }
      for (Inst* i = bb_body->instructions.head;;) {
        if (auto x = dyn_cast<PhiInst>(i)) {
          Inst* next = x->next;
          x->ReplaceAllUseWith(x->incoming_values[!idx_in_body].value);
          bb_body->instructions.Remove(x);
          delete x;
          i = next;
        } else
          break;
      }
      continue;
    }
  // 特判失败了，还是展开 2 次
  normal_unroll:
    Value* new_i0 = new BinaryInst(
        Value::Tag::Add, (&cond0->lhs)[cond0_i0].value, ConstValue::Get(step),
        cond0 ? static_cast<Inst*>(cond0) : static_cast<Inst*>(br0));
    br0->cond.Set(new BinaryInst(cond0->tag, cond0_i0 == 0 ? new_i0 : old_n,
                                 cond0_i0 == 0 ? old_n : new_i0, cond0));

    for (Inst* i = first_non_phi;; i = i->next) {
      clone_inst(i, bb_body, map);
      if (i == orig_last) break;
    }
    bb_body->instructions.InsertAtEnd(br);

    Value* new_ix =
        new BinaryInst(Value::Tag::Add, get((&cond->lhs)[cond_ix].value),
                       ConstValue::Get(step), br);
    Value* new_cond = new BinaryInst(cond->tag, cond_ix == 0 ? new_ix : old_n,
                                     cond_ix == 0 ? old_n : new_ix, br);
    br->cond.Set(new_cond);

    auto bb_if = new BasicBlock;
    auto bb_last = new BasicBlock;
    f->bb.InsertAfter(bb_if, bb_body);
    f->bb.InsertAfter(bb_last, bb_if);
    br0->right = bb_if, bb_if->pred.push_back(bb_cond);
    br->right = bb_if, bb_if->pred.push_back(bb_body);
    bb_last->pred.push_back(bb_if);
    {
      // PhiInst 的构造函数要求 instructions 非空，所以先插入最后的指令
      auto if_cond = new BinaryInst(cond->tag, nullptr, nullptr, bb_if);
      new BranchInst(if_cond, bb_last, bb_end, bb_if);
      // 这一步是构造 bb_if 中的 phi，它来自 bb_body 和 bb_end 的 phi
      // (bb_body 和 bb_end 的 phi
      // 并不一定完全一样，有可能一个值只在循环内用到，也有可能一个循环内定义的值循环中却没有用到)
      // 循环 1 构造来自 bb_body 的 phi，顺便将来自 bb_body 的 phi 的来自
      // bb_body 的值修改为新值 循环 2 修改 map，将来自 bb_body 的 phi
      // 映射到刚刚插入的 phi 循环 3 构造来自 bb_end 的 phi 循环 1 和 2
      // 不能合并，否则违背了 phi 的 parallel 的特性，当 bb_body 中的一个 phi
      // 作为另一个 phi 的操作数时，就可能出错
      for (Inst* i = bb_body->instructions.head;; i = i->next) {
        if (auto x = dyn_cast<PhiInst>(i)) {
          Value *from_cond = x->incoming_values[!idx_in_body].value,
                *from_body = get(x->incoming_values[idx_in_body].value);
          x->incoming_values[idx_in_body].Set(from_body);
          auto p = new PhiInst(if_cond);
          p->incoming_values[0].Set(from_cond);
          p->incoming_values[1].Set(from_body);
        } else
          break;
      }
      for (Inst *i = bb_body->instructions.head, *i1 = bb_if->instructions.head;
           ; i = i->next, i1 = i1->next) {
        if (isa<PhiInst>(i))
          map.find(i)->second = i1;
        else
          break;
      }
      for (Inst* i = bb_end->instructions.head;; i = i->next) {
        if (auto x = dyn_cast<PhiInst>(i)) {
          Value *from_cond = x->incoming_values[!idx_in_end].value,
                *from_body = get(x->incoming_values[idx_in_end].value);
          bool found = false;
          for (Inst* j = bb_if->instructions.head; !found; j = j->next) {
            if (auto y = dyn_cast<PhiInst>(j)) {
              if (y->incoming_values[0].value == from_cond &&
                  y->incoming_values[1].value == from_body) {
                x->incoming_values[!idx_in_end].Set(y);
                found = true;
              }
            } else
              break;
          }
          if (!found) {
            auto p = new PhiInst(if_cond);
            p->incoming_values[0].Set(from_cond);
            p->incoming_values[1].Set(from_body);
            x->incoming_values[!idx_in_end].Set(p);
          }
        } else
          break;
      }
      (&if_cond->lhs)[cond_ix].Set(get(phi_ix));
      (&if_cond->lhs)[!cond_ix].Set(old_n);
    }
    bb_end->pred[!idx_in_end] = bb_if;
    bb_end->pred[idx_in_end] = bb_last;
    for (Inst* i = first_non_phi;; i = i->next) {
      clone_inst(i, bb_last, map);
      if (i == orig_last) break;
    }
    new JumpInst(bb_end, bb_last);
    for (Inst* i = bb_end->instructions.head; i; i = i->next) {
      if (auto x = dyn_cast<PhiInst>(i)) {
        x->incoming_values[idx_in_end].Set(
            get(x->incoming_values[idx_in_end].value));
      } else
        break;
    }
  }
}