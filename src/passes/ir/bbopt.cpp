#include "passes/ir/bbopt.hpp"

static void dfs(BasicBlock* bb) {
  if (!bb->vis) {
    bb->vis = true;
    for (BasicBlock* x : bb->succ()) {
      if (x) dfs(x);
    }
  }
}

// 如果发生了将入度为 1 的 bb 的 phi 直接替换成这个值的优化，则返回
// true，gvn_gcm 需要这个信息，因为这可能产生新的优化机会
// 如果不这样做，最终交给后端的 ir 可能包含常量间的二元运算，这是后端不允许的
bool bbopt(IrFunc* f) {
  bool changed;
  do {
    changed = false;
    // 这个循环本来只是为了消除 if (常数)，没有必要放在 do
    // while 里的，但是在这里消除 if (x) br a else br a 也比较方便
    // 后面这种情形会被下面的循环引入，所以这个循环也放在 do while 里
    for (BasicBlock* bb = f->bb.head; bb; bb = bb->next) {
      if (auto x = dyn_cast<BranchInst>(bb->insts.tail)) {
        BasicBlock* deleted = nullptr;
        if (auto cond = dyn_cast<ConstValue>(x->cond.value)) {
          new JumpInst(cond->imm ? x->left : x->right, bb);
          deleted = cond->imm ? x->right : x->left;
        } else if (x->left ==
                   x->right) {  // 可能被消除以 jump 结尾的空基本块引入
          new JumpInst(x->left, bb);
          deleted = x->right;
          changed = true;  // 可能引入新的以 jump 结尾的空基本块
        }
        if (deleted) {
          bb->insts.Remove(x);
          delete x;
          u32 idx = std::find(deleted->pred.begin(), deleted->pred.end(), bb) -
                    deleted->pred.begin();
          deleted->pred.erase(deleted->pred.begin() + idx);
          for (Inst* i = deleted->insts.head;; i = i->next) {
            if (auto phi = dyn_cast<PhiInst>(i))
              phi->incoming_values.erase(phi->incoming_values.begin() + idx);
            else
              break;
          }
        }
      }
    }
    // 这个循环消除以 jump 结尾的空基本块
    // 简单起见不考虑第一个 bb，因为第一个 bb 是 entry，把它删了需要把新的 entry
    // 移动到第一个来
    for (BasicBlock* bb = f->bb.head->next; bb;) {
      BasicBlock* next = bb->next;
      // 要求 target != bb，避免去掉空的死循环
      if (auto x = dyn_cast<JumpInst>(bb->insts.tail);
          x && x->next != bb && bb->insts.head == bb->insts.tail) {
        BasicBlock* target = x->next;
        // 如果存在一个 pred，它以 BranchInst 结尾，且 left 或 right 已经为
        // target，且 target 中存在 phi，则不能把另一个也变成 bb 例如 bb1: { b =
        // a + 1; if (x) br bb2 else br bb3 } bb2: { br bb3; } bb3: { c = phi
        // [a, bb1] [b bb2] } 这时 bb2 起到了一个区分 phi 来源的作用
        if (isa<PhiInst>(target->insts.head)) {
          for (BasicBlock* p : bb->pred) {
            if (auto br = dyn_cast<BranchInst>(p->insts.tail)) {
              if (br->left == target || br->right == target) goto end;
            }
          }
        }
        u32 idx = std::find(target->pred.begin(), target->pred.end(), bb) -
                  target->pred.begin();
        target->pred.erase(target->pred.begin() + idx);
        for (BasicBlock* p : bb->pred) {
          auto succ = p->succ_ref();
          **std::find_if(succ.begin(), succ.end(),
                         [bb](BasicBlock** y) { return *y == bb; }) = target;
          target->pred.push_back(p);
        }
        u32 n_pred = bb->pred.size();
        for (Inst* i = target->insts.head;; i = i->next) {
          if (auto phi = dyn_cast<PhiInst>(i)) {
            Value* v = phi->incoming_values[idx].value;
            phi->incoming_values.erase(phi->incoming_values.begin() + idx);
            for (u32 j = 0; j < n_pred; ++j) {
              phi->incoming_values.emplace_back(v, phi);
            }
          } else
            break;
        }
        f->bb.Remove(bb);
        delete bb;
        changed = true;
      }
    end:;
      bb = next;
    }
  } while (changed);

  f->clear_all_vis();
  dfs(f->bb.head);
  // 不可达的 bb 仍然可能有指向可达的 bb 的边，需要删掉目标 bb 中的 pred 和 phi
  // 中的这一项
  for (BasicBlock* bb = f->bb.head; bb; bb = bb->next) {
    if (!bb->vis) {
      for (BasicBlock* s : bb->succ()) {
        if (s && s->vis) {
          u32 idx =
              std::find(s->pred.begin(), s->pred.end(), bb) - s->pred.begin();
          s->pred.erase(s->pred.begin() + idx);
          for (Inst* i = s->insts.head;; i = i->next) {
            if (auto x = dyn_cast<PhiInst>(i))
              x->incoming_values.erase(x->incoming_values.begin() + idx);
            else
              break;
          }
        }
      }
    }
  }
  for (BasicBlock* bb = f->bb.head; bb;) {
    BasicBlock* next = bb->next;
    if (!bb->vis) {
      f->bb.Remove(bb);
      delete bb;
    }
    bb = next;
  }

  bool inst_changed = false;

  for (BasicBlock* bb = f->bb.head; bb; bb = bb->next) {
    if (bb->pred.size() == 1) {
      for (Inst* i = bb->insts.head;;) {
        Inst* next = i->next;
        if (auto x = dyn_cast<PhiInst>(i)) {
          inst_changed = true;
          assert(x->incoming_values.size() == 1);
          x->replaceAllUseWith(x->incoming_values[0].value);
          bb->insts.Remove(x);
          delete x;
        } else
          break;
        i = next;
      }
    }
  }

  // 合并无条件跳转，这对性能没有影响，但是可以让其他优化更好写
  for (BasicBlock* bb = f->bb.head; bb; bb = bb->next) {
  again:
    if (auto x = dyn_cast<JumpInst>(bb->insts.tail)) {
      BasicBlock* target = x->next;
      if (target->pred.size() == 1) {
        assert(!isa<PhiInst>(target->insts.head));
        for (Inst* i = target->insts.head; i;) {
          Inst* next = i->next;
          target->insts.Remove(i);
          bb->insts.InsertBefore(i, x);
          i->bb = bb;
          i = next;
        }
        bb->insts.Remove(x);
        delete x;
        for (BasicBlock* s : bb->succ()) {
          if (s) {
            *std::find(s->pred.begin(), s->pred.end(), target) = bb;
          }
        }
        f->bb.Remove(target);
        delete target;
        goto again;
      }
    }
  }

  return inst_changed;
}