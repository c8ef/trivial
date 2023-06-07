#include "passes/ir/dce.hpp"

#include "passes/ir/remove_useless_loop.hpp"

static void dfs(std::unordered_set<Inst*>& vis, Inst* i) {
  if (!vis.insert(i).second) return;
  for (auto [it, end] = i->Operands(); it < end; ++it) {
    if (auto x = dyn_cast_nullable<Inst>(it->value)) dfs(vis, x);
  }
}

void dce(IrFunc* f) {
  std::unordered_set<Inst*> vis;
again:
  vis.clear();
  for (BasicBlock* bb = f->bb.head; bb; bb = bb->next) {
    for (Inst* i = bb->insts.head; i; i = i->next) {
      if (i->HasSideEffect()) dfs(vis, i);
    }
  }
  // 无用的指令间可能相互使用，所以需要先清空 operand，否则 delete
  // 的时候会试图维护已经 delete 掉的指令的 uses
  for (BasicBlock* bb = f->bb.head; bb; bb = bb->next) {
    for (Inst* i = bb->insts.head; i; i = i->next) {
      if (vis.find(i) == vis.end()) {
        for (auto [it, end] = i->Operands(); it < end; ++it) it->Set(nullptr);
      }
    }
  }
  for (BasicBlock* bb = f->bb.head; bb; bb = bb->next) {
    for (Inst* i = bb->insts.head; i;) {
      Inst* next = i->next;
      if (vis.find(i) == vis.end()) {
        bb->insts.Remove(i);
        i->DeleteValue();
      }
      i = next;
    }
  }
  if (remove_useless_loop(f)) goto again;
}