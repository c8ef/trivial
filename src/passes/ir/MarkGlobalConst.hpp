#include "structure/ast.hpp"
#include "structure/ir.hpp"

inline bool HasStore(Value* v) {
  for (Use* u = v->uses.head; u; u = u->next) {
    if (isa<LoadInst>(u->user)) continue;
    if (isa<StoreInst>(u->user)) return true;
    if (auto* x = dyn_cast<GetElementPtrInst>(u->user)) {
      if (HasStore(x)) return true;
    } else if (auto* x = dyn_cast<CallInst>(u->user)) {
      // TODO(opt): do more analysis on function parameter
      if (x->HasSideEffect()) return true;
    } else {
      // now memory address can only accessed by load/store/call/GEP
      UNREACHABLE();
    }
  }
  return false;
}

inline void MarkGlobalConst(IrProgram* p) {
  for (Decl* d : p->glob_decl) {
    if (!d->is_const && !HasStore(d->value)) {
      // do not inline global arrays
      if (!d->dims.empty()) continue;
      d->is_const = true;
      DEBUG("mark global variable {} as const variable", d->name);
    }
  }
}
