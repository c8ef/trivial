#pragma once

#include "structure/ast.hpp"
#include "structure/ir.hpp"

// this pass rely on ComputeCallGraph
inline void RemoveUnusedFunction(IrProgram* p) {
  for (auto* f = p->func.head; f; f = f->next) {
    if (f->caller_func.empty() && f->func->name != "main") {
      DEBUG("function {} is not used, removed", f->func->name);
      p->func.Remove(f);
    }
  }
}
