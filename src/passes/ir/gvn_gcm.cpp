#include "passes/ir/gvn_gcm.hpp"

#include "passes/ir/BasicBlockOpt.hpp"
#include "passes/ir/CFG.hpp"
#include "passes/ir/dce.hpp"
#include "passes/ir/memdep.hpp"
#include "structure/ast.hpp"
#include "structure/op.hpp"

using VN = std::vector<std::pair<Value*, Value*>>;

static Value* vn_of(VN& vn, Value* x);

static Value* find_eq(VN& vn, BinaryInst* x) {
  using namespace op;
  Op t1 = (Op)x->tag;
  Value *l1 = vn_of(vn, x->lhs.value), *r1 = vn_of(vn, x->rhs.value);
  // 不能用迭代器，因为 vn_of 会往 vn 中 push 元素
  for (u32 i = 0; i < vn.size(); ++i) {
    auto [k, v] = vn[i];
    // 这时的 vn 已经插入了{x, x}，所以如果遍历的时候遇到 x 要跳过
    if (auto y = dyn_cast<BinaryInst>(k); y && y != x) {
      Op t2 = (Op)y->tag;
      Value *l2 = vn_of(vn, y->lhs.value), *r2 = vn_of(vn, y->rhs.value);
      bool same = (t1 == t2 && ((l1 == l2 && r1 == r2) ||
                                (l1 == r2 && r1 == l2 &&
                                 (t1 == Add || t1 == Mul || t1 == Eq ||
                                  t1 == Ne || t1 == And || t1 == Or)))) ||
                  (l1 == r2 && r1 == l2 && isrev(t1, t2));
      if (same) return v;
    }
  }
  return x;
}

// GetElementPtrInst 和 LoadInst 的 find_eq 中，对 x->arr.value
// 的递归搜索最终会终止于 AllocaInst, ParamRef, GlobalRef，它们直接用指针比较
static Value* find_eq(VN& vn, GetElementPtrInst* x) {
  for (u32 i = 0; i < vn.size(); ++i) {
    auto [k, v] = vn[i];
    if (auto y = dyn_cast<GetElementPtrInst>(k); y && y != x) {
      bool same = vn_of(vn, x->arr.value) == vn_of(vn, y->arr.value) &&
                  vn_of(vn, x->index.value) == vn_of(vn, y->index.value);
      if (same) return v;
    }
  }
  return x;
}

static Value* find_eq(VN& vn, LoadInst* x) {
  for (u32 i = 0; i < vn.size(); ++i) {
    auto [k, v] = vn[i];
    if (auto y = dyn_cast<LoadInst>(k); y && y != x) {
      bool same = vn_of(vn, x->arr.value) == vn_of(vn, y->arr.value) &&
                  vn_of(vn, x->index.value) == vn_of(vn, y->index.value) &&
                  x->mem_token.value == y->mem_token.value;
      if (same) return v;
    } else if (auto y = dyn_cast<StoreInst>(k)) {
      // a[0] = 1; b = a[0] 可以变成 b = 1
      bool same =
          vn_of(vn, x->arr.value) == vn_of(vn, y->arr.value) &&
          vn_of(vn, x->index.value) == vn_of(vn, y->index.value) &&
          x->mem_token.value == y;  // 这意味着这个 store dominates 了这个 load
      if (same) return y->data.value;
    }
  }
  return x;
}

static bool is_pure_call(Inst* i) {
  auto x = dyn_cast<CallInst>(i);
  return x && x->func->Pure() &&
         std::none_of(x->args.begin(), x->args.end(), [](Use& arg) {
           return isa<GetElementPtrInst>(arg.value);
         });
}

static Value* find_eq(VN& vn, CallInst* x) {
  if (!is_pure_call(x)) return x;
  for (u32 i = 0; i < vn.size(); ++i) {
    auto [k, v] = vn[i];
    if (auto y = dyn_cast<CallInst>(k); y && y->func == x->func) {
      assert(x->args.size() == y->args.size());
      for (u32 j = 0; j < x->args.size(); ++j) {
        if (vn_of(vn, x->args[j].value) != vn_of(vn, y->args[j].value))
          goto fail;
      }
      return v;
    fail:;
    }
  }
  return x;
}

static Value* vn_of(VN& vn, Value* x) {
  auto it =
      std::find_if(vn.begin(), vn.end(),
                   [x](std::pair<Value*, Value*> kv) { return kv.first == x; });
  if (it != vn.end()) return it->second;
  // 此时没有指针相等的，但是仍然要找是否存在实际相等的，如果没有的话它的 vn
  // 就是 x
  u32 idx = vn.size();
  vn.emplace_back(x, x);
  if (auto y = dyn_cast<BinaryInst>(x))
    vn[idx].second = find_eq(vn, y);
  else if (auto y = dyn_cast<GetElementPtrInst>(x))
    vn[idx].second = find_eq(vn, y);
  else if (auto y = dyn_cast<LoadInst>(x))
    vn[idx].second = find_eq(vn, y);
  else if (auto y = dyn_cast<CallInst>(x))
    vn[idx].second = find_eq(vn, y);
  // 其余情况一定要求指针相等
  return vn[idx].second;
}

// 把形如 b = a + 1; c = b + 1 的 c 转化成 a + 2
// 乘法：b = a * C1, c = b * C2 => a * (C1 * C2)
// 加减总共 9 种情况，b 和 c 都可以是 Add, Sub,
// Rsb，首先把 Sub 都变成 Add 负值，剩下四种情况 a + C1; b + C2 => a + (C1 + C2)
// a + C1; rsb b C2 => rsb a (C2 - C1) rsb a C1; b + C2 => rsb a (C1 + C2) rsb a
// C1; rsb b C2 => a + (C2 - C1) 故两个操作符相同时结果为 Add，否则为 Rsb; c 为
// Add 是 C2 前是正号，否则是负号
static void try_fold_lhs(BinaryInst* x) {
  if (auto r = dyn_cast<ConstValue>(x->rhs.value)) {
    if (x->tag == Value::Tag::Sub)
      x->rhs.Set(r = ConstValue::get(-r->imm)), x->tag = Value::Tag::Add;
    if (auto l = dyn_cast<BinaryInst>(x->lhs.value)) {
      if (auto lr = dyn_cast<ConstValue>(l->rhs.value)) {
        if (l->tag == Value::Tag::Sub)
          l->rhs.Set(lr = ConstValue::get(-lr->imm)), l->tag = Value::Tag::Add;
        if ((x->tag == Value::Tag::Add || x->tag == Value::Tag::Rsb) &&
            (l->tag == Value::Tag::Add || l->tag == Value::Tag::Rsb)) {
          x->lhs.Set(l->lhs.value);
          x->rhs.Set(ConstValue::get(
              r->imm + (x->tag == Value::Tag::Add ? lr->imm : -lr->imm)));
          x->tag = (x->tag == Value::Tag::Add) == (l->tag == Value::Tag::Add)
                       ? Value::Tag::Add
                       : Value::Tag::Rsb;
        } else if (x->tag == Value::Tag::Mul && r->tag == Value::Tag::Mul) {
          x->lhs.Set(l->lhs.value);
          x->rhs.Set(ConstValue::get(lr->imm * r->imm));
        }
      }
    }
  }
}

// 把 i 放到 new_bb 的末尾。这个 bb 中的位置不重要，因为后续还会再调整它在 bb
// 中的位置
static void transfer_inst(Inst* i, BasicBlock* new_bb) {
  i->bb->instructions.Remove(i);
  i->bb = new_bb;
  new_bb->instructions.InsertBefore(i, new_bb->instructions.tail);
}

// 目前只考虑移动 BinaryInst 的位置，其他都不允许移动
static void schedule_early(std::unordered_set<Inst*>& vis, BasicBlock* entry,
                           Inst* i) {
  auto schedule_op = [&vis, entry](Inst* x, Value* op) {
    if (auto op1 = dyn_cast<Inst>(op)) {
      schedule_early(vis, entry, op1);
      if (x->bb->dom_level < op1->bb->dom_level) {
        transfer_inst(x, op1->bb);
      }
    }
  };
  if (vis.insert(i).second) {
    if (auto x = dyn_cast<BinaryInst>(i)) {
      transfer_inst(x, entry);
      for (Use* op : {&x->lhs, &x->rhs}) {
        schedule_op(x, op->value);
      }
    } else if (auto x = dyn_cast<GetElementPtrInst>(i)) {
      transfer_inst(x, entry);
      schedule_op(x, x->arr.value);
      schedule_op(x, x->index.value);
    } else if (auto x = dyn_cast<LoadInst>(i)) {
      transfer_inst(x, entry);
      schedule_op(x, x->arr.value);
      schedule_op(x, x->index.value);
      schedule_op(x, x->mem_token.value);
    } else if (is_pure_call(i)) {
      transfer_inst(i, entry);
      for (Use& arg : static_cast<CallInst*>(i)->args) {
        schedule_op(i, arg.value);
      }
    }
  }
}

static BasicBlock* find_lca(BasicBlock* a, BasicBlock* b) {
  while (b->dom_level < a->dom_level) a = a->idom;
  while (a->dom_level < b->dom_level) b = b->idom;
  while (a != b) {
    a = a->idom;
    b = b->idom;
  }
  return a;
}

static void schedule_late(std::unordered_set<Inst*>& vis, LoopInfo& info,
                          Inst* i) {
  if (vis.insert(i).second) {
    if (isa<BinaryInst>(i) || isa<GetElementPtrInst>(i) || isa<LoadInst>(i) ||
        is_pure_call(i)) {
      BasicBlock* lca = nullptr;
      for (Use* u = i->uses.head; u; u = u->next) {
        Inst* u1 = u->user;
        schedule_late(vis, info, u1);
        BasicBlock* use = u1->bb;
        // MemPhiInst 的注释中解释了，这里把 MemPhiInst 当成 PhiInst 用
        if (isa<PhiInst>(u1) || isa<MemPhiInst>(u1)) {
          auto y = static_cast<PhiInst*>(u1);
          auto it = std::find_if(y->incoming_values.begin(),
                                 y->incoming_values.end(), [u](const Use& u2) {
                                   // 这里必须比较 Use 的地址，而不能比较
                                   // u2.value
                                   // == i 因为一个 Phi
                                   // 可以有多个相同的输入，例如 phi [0, bb1]
                                   // [%x0, bb2] [%x0, bb3] 如果找 u2.value ==
                                   // i 的，那么两次查找都会返回 [%x0,
                                   // bb2]，导致后面认为只有一个 bb 用到了%x0
                                   return &u2 == u;
                                 });
          use = y->IncomingBbs()[it - y->incoming_values.begin()];
        }
        lca = lca ? find_lca(lca, use) : use;
      }
      // lca
      // 为空即没有人使用它，这并不是什么有意义的情形，而且也没法知道这条指令该放在哪里了
      // 因此在 gvn_gcm pass 前需要先运行一遍 dce pass，保证没有这种情形
      assert(lca != nullptr);
      BasicBlock* best = lca;
      u32 best_loop_depth = info.DepthOf(best);
      // 论文里是 while (lca !=
      // i->bb)，但是我觉得放在 x->bb 也是可以的，所以改成了考虑完 lca
      // 后再判断是否等于 x->bb
      while (true) {
        u32 cur_loop_depth = info.DepthOf(lca);
        if (cur_loop_depth < best_loop_depth) {
          best = lca;
          best_loop_depth = cur_loop_depth;
        }
        if (lca == i->bb) break;
        lca = lca->idom;
      }
      transfer_inst(i, best);
      for (Inst* j = best->instructions.head; j; j = j->next) {
        if (!isa<PhiInst>(j)) {
          for (Use* u = i->uses.head; u; u = u->next) {
            if (u->user == j) {
              best->instructions.Remove(i);
              best->instructions.InsertBefore(i, j);
              goto out;
            }
          }
        }
      }
    out:;
    }
  }
}

// 现在的依赖关系有点复杂，gvn_gcm 的第一阶段需要 memdep，第二阶段需要 memdep 和
// dce, 而 dce 会清除 memdep 的结果 不是很方便在 pass manager
// 里表示这种关系，所以只能在第一阶段前手动调用 memdep，第二阶段前手动依次调用
// dce 和 memdep
void gvn_gcm(IrFunc* f) {
  BasicBlockOpt(f);
again:
  BasicBlock* entry = f->bb.head;
  // 阶段 1，gvn
  compute_memdep(f);
  std::vector<BasicBlock*> rpo = ComputeRPO(f);
  VN vn;
  auto replace = [&vn](Inst* o, Value* n) {
    if (o != n) {
      o->ReplaceAllUseWith(n);
      o->bb->instructions.Remove(o);
      auto it = std::find_if(
          vn.begin(), vn.end(),
          [o](std::pair<Value*, Value*> kv) { return kv.first == o; });
      if (it != vn.end()) {
        std::swap(*it, vn.back());
        vn.pop_back();
      }
      o->DeleteValue();
    }
  };
  for (BasicBlock* bb : rpo) {
    for (Inst* i = bb->instructions.head; i;) {
      Inst* next = i->next;
      if (auto x = dyn_cast<BinaryInst>(i)) {
        if (isa<ConstValue>(x->lhs.value) && x->SwapOperand()) {
          DEBUG("IMM operand moved from lhs to rhs");
        }
        auto l = dyn_cast<ConstValue>(x->lhs.value),
             r = dyn_cast<ConstValue>(x->rhs.value);
        // for most instructions reach here, rhs is IMM
        if (l && r) {
          // both constant, evaluate and eliminate
          replace(x, ConstValue::get(op::Eval((op::Op)x->tag, l->imm, r->imm)));
        } else {
          try_fold_lhs(x);
          if (auto value = x->OptimizedValue()) {
            // can be (arithmetically) replaced with one single value (constant
            // or one side of operands)
            replace(x, value);
          } else {
            replace(x, vn_of(vn, x));
          }
        }
      } else if (auto x = dyn_cast<PhiInst>(i)) {
        Value* fst = vn_of(vn, x->incoming_values[0].value);
        bool all_same = true;
        for (u32 i = 1, sz = x->incoming_values.size(); i < sz && all_same;
             ++i) {
          all_same = fst == vn_of(vn, x->incoming_values[i].value);
        }
        if (all_same) replace(x, fst);
      } else if (auto x = dyn_cast<LoadInst>(i)) {
        bool replaced = false;
        if (x->lhs_sym->is_glob && x->lhs_sym->is_const) {
          replaced = true;
          u32 offset = 0;
          AccessInst* a = x;
          for (u32 dim_pos = x->lhs_sym->dims.size(); dim_pos >= 1; --dim_pos) {
            if (auto idx = dyn_cast<ConstValue>(a->index.value)) {
              assert(a != nullptr);
              offset += idx->imm * (dim_pos == x->lhs_sym->dims.size()
                                        ? 1
                                        : x->lhs_sym->dims[dim_pos]->result);
              a = dyn_cast<GetElementPtrInst>(a->arr.value);
            } else {
              replaced = false;
              break;
            }
          }
          if (replaced)
            replace(x, ConstValue::get(
                           x->lhs_sym->flatten_init_list[offset]->result));
        }
        if (!replaced) replace(i, vn_of(vn, i));
      } else if (isa<GetElementPtrInst>(i) || is_pure_call(i)) {
        replace(i, vn_of(vn, i));
      } else if (isa<StoreInst>(i)) {
        // 这里没有必要做替换，把 StoreInst 放进 vn 的目的是让 LoadInst 可以用
        // store 的右手项 vn 中一定不含这个 i，因为没有人用到 StoreInst(唯一用到
        // StoreInst 的地方是 mem_token，但是没有加入 vn)
        vn.emplace_back(i, i);
      }
      // 没有必要主动把其他指令加入 vn，如果它们被用到的话自然会被加入的
      i = next;
    }
  }
  clear_memdep(f), dce(f), compute_memdep(f);
  // 阶段 2，gcm
  LoopInfo info = ComputeLoopInfo(f);
  std::vector<Inst*> instructions;
  for (BasicBlock* bb = entry; bb; bb = bb->next) {
    for (Inst* i = bb->instructions.head; i; i = i->next)
      instructions.push_back(i);
  }
  std::unordered_set<Inst*> vis;
  for (Inst* i : instructions) schedule_early(vis, entry, i);
  vis.clear();
  for (Inst* i : instructions) schedule_late(vis, info, i);
  // fixme:
  // 其实本质上这个算法是没办法决定一个 bb
  // 内的任何顺序的，需要别的调度策略，这里简单做一个，激活 cmp
  // + branch 的优化
  for (BasicBlock* bb = entry; bb; bb = bb->next) {
    if (auto x = dyn_cast<BranchInst>(bb->instructions.tail)) {
      if (x->cond.value->tag >= Value::Tag::Lt &&
          x->cond.value->tag <= Value::Tag::Ne) {
        auto c = static_cast<Inst*>(x->cond.value);
        if (c->bb == bb &&
            c->uses.head == c->uses.tail) {  // 要求只被这个 branch 使用
          bb->instructions.Remove(c);
          bb->instructions.InsertBefore(c, x);
        }
      }
    }
  }
  clear_memdep(f);
  if (BasicBlockOpt(f)) goto again;
}
