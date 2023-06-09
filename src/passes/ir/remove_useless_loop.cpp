#include "passes/ir/remove_useless_loop.hpp"

#include "passes/ir/CFG.hpp"

bool remove_useless_loop(IrFunc* f) {
  std::vector<Loop*> deepest = ComputeLoopInfo(f).DeepestLoops();
  bool changed = false;
  for (Loop* l : deepest) {
    // 变量需要提前定义，因为 goto 不能跳过变量初始化
    BasicBlock* pre_header = nullptr;
    BasicBlock* unique_exit = nullptr;
    std::vector<BasicBlock*> exiting;  // 包含于 unique_exit 的 pred
    for (BasicBlock* p : l->bbs[0]->pred) {
      if (std::find(l->bbs.begin(), l->bbs.end(), p) == l->bbs.end()) {
        if (pre_header) goto fail;  // 有多于一个从循环外跳转到循环头的 bb，失败
        pre_header = p;
      }
    }
    assert(pre_header != nullptr);
    for (BasicBlock* bb : l->bbs) {
      for (BasicBlock* s : bb->Succ()) {
        if (s && std::find(l->bbs.begin(), l->bbs.end(), s) == l->bbs.end()) {
          if (unique_exit && unique_exit != s)
            goto fail;  // 有多于一个出口 bb，失败
          unique_exit = s;
          exiting.push_back(bb);
        }
      }
    }
    if (!unique_exit) goto fail;  // 没有出口，确定是死循环，不考虑
    for (Inst* i = unique_exit->instructions.head;; i = i->next) {
      if (auto x = dyn_cast<PhiInst>(i)) {
        Value* fst =
            x->incoming_values[std::find(unique_exit->pred.begin(),
                                         unique_exit->pred.end(), exiting[0]) -
                               unique_exit->pred.begin()]
                .value;
        for (auto it = exiting.begin() + 1; it < exiting.end(); ++it) {
          if (fst !=
              x->incoming_values[std::find(unique_exit->pred.begin(),
                                           unique_exit->pred.end(), *it) -
                                 unique_exit->pred.begin()]
                  .value) {
            goto fail;  // 循环出口处 phi 依赖于从循环中哪个 bb 退出，失败
          }
        }
      } else
        break;
    }
    for (BasicBlock* bb : l->bbs) {
      for (Inst* i = bb->instructions.head; i; i = i->next) {
        if (isa<ReturnInst>(i) || isa<StoreInst>(i)) goto fail;
        if (auto x = dyn_cast<CallInst>(i); x && x->func->has_side_effect)
          goto fail;
        // 检查是否被循环外的指令使用
        // LLVM 的 LoopDeletion
        // Pass 不检查这个，而是在上面一个循环中，检查 fst 是否是循环中定义的
        // 这是因为它保证当前的 IR 是 LCSSA
        // 的形式，任何循环中定义的值想要被被外界使用，都需要经过 PHI
        // 我们没有这个保证，所以不能这样检查
        for (Use* u = i->uses.head; u; u = u->next) {
          if (std::find(l->bbs.begin(), l->bbs.end(), u->user->bb) ==
              l->bbs.end())
            goto fail;
        }
      }
    }

    DEBUG("Removing useless loop");
    changed = true;
    {
      bool found = false;
      for (BasicBlock** s : pre_header->SuccRef()) {
        if (s && *s == l->bbs[0]) {
          found = true;
          *s = unique_exit;
          break;
        }
      }
      assert(found);
    }
    unique_exit->pred.push_back(pre_header);
    for (auto it = exiting.begin(); it < exiting.end(); ++it) {
      u32 idx =
          std::find(unique_exit->pred.begin(), unique_exit->pred.end(), *it) -
          unique_exit->pred.begin();
      unique_exit->pred.erase(unique_exit->pred.begin() + idx);
      for (Inst* i = unique_exit->instructions.head;; i = i->next) {
        if (auto x = dyn_cast<PhiInst>(i)) {
          if (it == exiting.begin()) {
            x->incoming_values.emplace_back(x->incoming_values[idx].value, x);
          }
          x->incoming_values.erase(x->incoming_values.begin() + idx);
        } else
          break;
      }
    }
    for (BasicBlock* bb : l->bbs) {
      for (Inst* i = bb->instructions.head; i; i = i->next) {
        for (auto [it, end] = i->Operands(); it < end; ++it) it->Set(nullptr);
      }
      f->bb.Remove(bb);
      delete bb;
    }
  fail:;
  }
  return changed;
}