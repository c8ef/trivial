#pragma once

#include "structure/ast.hpp"
#include "structure/ir.hpp"

struct SsaContext {
  IrProgram* program;
  IrFunc* func;
  BasicBlock* bb;
  // bb stack for (continue, break)
  std::vector<std::pair<BasicBlock*, BasicBlock*>> loop_stk;
};

IrProgram* ConvertSSA(Program& p);
