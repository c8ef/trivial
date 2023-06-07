#pragma once

#include "structure/ast.hpp"
#include "structure/ir.hpp"

IrProgram* ConvertSSA(Program& p);
