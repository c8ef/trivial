#include "passes/ir/mark_global_const.hpp"

#include "structure/ast.hpp"

static bool has_store(Value* v) {
  for (Use* u = v->uses.head; u; u = u->next) {
    if (isa<LoadInst>(u->user))
      continue;
    else if (isa<StoreInst>(u->user))
      return true;
    else if (auto x = dyn_cast<GetElementPtrInst>(u->user)) {
      if (has_store(x)) return true;
    } else if (auto x = dyn_cast<CallInst>(u->user)) {
      // todo: 和 memdep 中的注释一样，这里可以更仔细地分析函数是否修改这个参数
      if (x->HasSideEffect()) return true;
    } else {
      // 目前地址只可能以 GetElementPtr 传递，到 Load, Store,
      // Call 为止，不可能被别的指令，如 Phi 之类的使用
      UNREACHABLE();
    }
  }
  return false;
}

void mark_global_const(IrProgram* p) {
  for (Decl* d : p->glob_decl) {
    if (!d->is_const && !has_store(d->value)) {
      // NOTE_OPT: we choose not to inline global arrays (which causes bad
      // performance on bitset)
      if (!d->dims.empty()) continue;
      d->is_const = true;
      auto global_const =
          "Marking global variable '" + std::string(d->name) + "' as const";
      DEBUG(global_const);
    }
  }
}
