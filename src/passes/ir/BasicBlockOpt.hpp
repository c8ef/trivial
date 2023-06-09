#include "structure/ir.hpp"

inline void BBODFS(BasicBlock* bb) {
  if (!bb->vis) {
    bb->vis = true;
    for (BasicBlock* x : bb->Succ()) {
      if (x) BBODFS(x);
    }
  }
}

// if occur a phi which has only one incoming value, replace it with the value
// then return true
inline bool BasicBlockOpt(IrFunc* f) {
  bool changed;
  do {
    changed = false;

    for (BasicBlock* bb = f->bb.head; bb; bb = bb->next) {
      if (auto* x = dyn_cast<BranchInst>(bb->instructions.tail)) {
        BasicBlock* deleted = nullptr;
        if (auto* cond = dyn_cast<ConstValue>(x->cond.value)) {
          // simplify if (ConstInt)
          new JumpInst(cond->imm ? x->left : x->right, bb);
          deleted = cond->imm ? x->right : x->left;
        } else if (x->left == x->right) {
          // simplify if(x) br a else br a
          new JumpInst(x->left, bb);
          deleted = x->right;
          changed = true;
        }
        if (deleted) {
          bb->instructions.Remove(x);
          delete x;
          i64 idx = std::find(deleted->pred.begin(), deleted->pred.end(), bb) -
                    deleted->pred.begin();
          deleted->pred.erase(deleted->pred.begin() + idx);
          for (Inst* i = deleted->instructions.head;; i = i->next) {
            if (auto* phi = dyn_cast<PhiInst>(i))
              phi->incoming_values.erase(phi->incoming_values.begin() + idx);
            else
              break;
          }
        }
      }
    }

    // simplify the empty block ended with jump
    // skip the entry block
    for (BasicBlock* bb = f->bb.head->next; bb;) {
      BasicBlock* next = bb->next;
      // if jump target equals itself than it is a empty infinite loop
      if (auto* x = dyn_cast<JumpInst>(bb->instructions.tail);
          x && x->next != bb &&
          bb->instructions.head == bb->instructions.tail) {
        BasicBlock* target = x->next;
        //                      +----------------+
        //                      | bb1            |
        //                      |                |
        //                      | b = a + 1;     |
        //      +---------------+ if (x) br bb2; |
        //      |               | else br bb3;   |
        //      |               |                |
        //      |               +---------+------+
        //      |                         |
        //      |                         |
        //      |                         |
        //      |                         |
        // +----v-----+                   |
        // |  bb2     |                   |
        // |          |                   |
        // |  br bb3; |                   |
        // +----+-----+                   |
        //      |                         |
        //      |                         |
        //      |                         |
        //      |                         |
        //      |              +----------v-----------------+
        //      |              | bb3                        |
        //      +-------------->                            |
        //                     | c = phi [a, bb1], [b, bb2] |
        //                     +----------------------------+
        // we cannot delete bb2 because phi use it
        if (isa<PhiInst>(target->instructions.head)) {
          for (BasicBlock* p : bb->pred) {
            if (auto* br = dyn_cast<BranchInst>(p->instructions.tail)) {
              if (br->left == target || br->right == target) goto end;
            }
          }
        }

        i64 idx = std::find(target->pred.begin(), target->pred.end(), bb) -
                  target->pred.begin();
        target->pred.erase(target->pred.begin() + idx);
        for (BasicBlock* p : bb->pred) {
          auto succ = p->SuccRef();
          **std::find_if(succ.begin(), succ.end(),
                         [bb](BasicBlock** y) { return *y == bb; }) = target;
          target->pred.push_back(p);
        }
        u64 n_pred = bb->pred.size();
        for (Inst* i = target->instructions.head;; i = i->next) {
          if (auto* phi = dyn_cast<PhiInst>(i)) {
            Value* v = phi->incoming_values[idx].value;
            phi->incoming_values.erase(phi->incoming_values.begin() + idx);
            for (u64 j = 0; j < n_pred; ++j) {
              phi->incoming_values.emplace_back(v, phi);
            }
          } else
            break;
        }
        f->bb.Remove(bb);
        delete bb;
        changed = true;
      }
    end:
      bb = next;
    }
  } while (changed);

  f->ClearAllVis();
  BBODFS(f->bb.head);
  // unavailable basic block can have edges to available basic block
  // delete them first
  for (BasicBlock* bb = f->bb.head; bb; bb = bb->next) {
    if (!bb->vis) {
      for (BasicBlock* s : bb->Succ()) {
        if (s && s->vis) {
          i64 idx =
              std::find(s->pred.begin(), s->pred.end(), bb) - s->pred.begin();
          s->pred.erase(s->pred.begin() + idx);
          for (Inst* i = s->instructions.head;; i = i->next) {
            if (auto* x = dyn_cast<PhiInst>(i))
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
      for (Inst* i = bb->instructions.head;;) {
        Inst* next = i->next;
        if (auto* x = dyn_cast<PhiInst>(i)) {
          inst_changed = true;
          assert(x->incoming_values.size() == 1);
          x->ReplaceAllUseWith(x->incoming_values[0].value);
          bb->instructions.Remove(x);
          delete x;
        } else
          break;
        i = next;
      }
    }
  }

  // combine unconditional jump
  for (BasicBlock* bb = f->bb.head; bb; bb = bb->next) {
  again:
    if (auto* x = dyn_cast<JumpInst>(bb->instructions.tail)) {
      BasicBlock* target = x->next;
      if (target->pred.size() == 1) {
        assert(!isa<PhiInst>(target->instructions.head));
        for (Inst* i = target->instructions.head; i;) {
          Inst* next = i->next;
          target->instructions.Remove(i);
          bb->instructions.InsertBefore(i, x);
          i->bb = bb;
          i = next;
        }
        bb->instructions.Remove(x);
        delete x;
        for (BasicBlock* s : bb->Succ()) {
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