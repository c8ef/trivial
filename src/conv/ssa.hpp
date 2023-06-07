#pragma once

#include "structure/ast.hpp"
#include "structure/ir.hpp"

struct SSAContext {
  IrProgram* program;
  IrFunc* func;
  BasicBlock* bb;
  // basic block stack for (continue, break)
  std::vector<std::pair<BasicBlock*, BasicBlock*>> loop_stk;
};

IrProgram* ConvertSSA(Program& program);
