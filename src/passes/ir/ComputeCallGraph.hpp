#pragma once

#include "structure/ast.hpp"
#include "structure/ir.hpp"

inline void ComputeCallGraph(IrProgram* p) {
  for (auto* f = p->func.head; f; f = f->next) {
    f->callee_func.clear();
    f->caller_func.clear();
    f->load_global = f->has_side_effect = f->builtin;
  }

  for (auto* f = p->func.head; f; f = f->next) {
    for (auto* bb = f->bb.head; bb; bb = bb->next) {
      for (auto* inst = bb->instructions.head; inst; inst = inst->next) {
        if (auto* x = dyn_cast<CallInst>(inst)) {
          f->callee_func.insert(x->func);
          x->func->caller_func.insert(f);
        } else if (auto* x = dyn_cast<LoadInst>(inst);
                   x && x->lhs_sym->is_glob) {
          f->load_global = true;
        } else if (auto* x = dyn_cast<StoreInst>(inst);
                   x && (x->lhs_sym->is_glob || x->lhs_sym->IsParamArray())) {
          f->has_side_effect = true;
        }
      }
    }
  }

  // propagate impure from callee to caller
  std::vector<IrFunc*> work_list;
  for (auto* f = p->func.head; f; f = f->next) {
    if (f->has_side_effect) {
      work_list.push_back(f);
    }
  }

  // function called by impure function is impure
  while (!work_list.empty()) {
    auto* f = work_list.back();
    work_list.pop_back();
    for (auto* caller : f->caller_func) {
      if (!caller->has_side_effect) {
        caller->has_side_effect = true;
        work_list.push_back(caller);
      }
    }
  }

  for (auto* f = p->func.head; f; f = f->next) {
    if (f->has_side_effect)
      DEBUG("function {} has side effect(impure)", f->func->name);
    else
      DEBUG("function {} has no side effect(pure)", f->func->name);
  }
}