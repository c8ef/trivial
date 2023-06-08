#include "conv/ssa.hpp"

#include "casting.hpp"
#include "structure/ast.hpp"

Value* ConvertExpr(SSAContext* ctx, Expr* expr) {
  if (auto* x = dyn_cast<Binary>(expr)) {
    auto* lhs = ConvertExpr(ctx, x->lhs);
    if (x->tag == Expr::Tag::Mod) {
      auto* rhs = ConvertExpr(ctx, x->rhs);
      // a % b := a - b * a / b
      auto* quotient = new BinaryInst(Value::Tag::Div, lhs, rhs, ctx->bb);
      auto* multiple = new BinaryInst(Value::Tag::Mul, rhs, quotient, ctx->bb);
      auto* remainder = new BinaryInst(Value::Tag::Sub, lhs, multiple, ctx->bb);
      return remainder;
    }
    if (x->tag == Expr::And || x->tag == Expr::Or) {
      // handle short circuit
      auto* rhs_bb = new BasicBlock;
      auto* after_bb = new BasicBlock;
      ctx->func->bb.InsertAtEnd(rhs_bb);
      if (x->tag == Expr::And) {
        new BranchInst(lhs, rhs_bb, after_bb, ctx->bb);
      }
      if (x->tag == Expr::Or) {
        auto* inv =
            new BinaryInst(Value::Tag::Eq, lhs, ConstValue::get(0), ctx->bb);
        new BranchInst(inv, rhs_bb, after_bb, ctx->bb);
      }
      // 这里需要 pred 的大小为 2，真正维护 pred 在最后才做，可以保证是 [当前
      // bb, rhs 的实际计算 bb] 的顺序 注意 rhs 的实际计算 bb 不一定就是
      // rhs_bb，因为 rhs 也可能是&& ||
      after_bb->pred.resize(2);
      ctx->bb = rhs_bb;
      auto* rhs = ConvertExpr(ctx, x->rhs);
      new JumpInst(after_bb, ctx->bb);
      ctx->func->bb.InsertAtEnd(after_bb);
      ctx->bb = after_bb;
      auto* inst = new PhiInst(ctx->bb);
      inst->incoming_values[0].Set(lhs);
      inst->incoming_values[1].Set(rhs);
      return inst;
    }
    auto* rhs = ConvertExpr(ctx, x->rhs);
    // happened to have same tag values
    auto* inst =
        new BinaryInst(static_cast<Value::Tag>(x->tag), lhs, rhs, ctx->bb);
    return inst;
  }
  if (auto* x = dyn_cast<IntConst>(expr)) {
    return ConstValue::get(x->val);
  }
  if (auto* x = dyn_cast<Index>(expr)) {
    // evaluate dimensions first
    std::vector<Value*> dims;
    dims.reserve(x->dims.size());
    for (auto& p : x->dims) {
      auto* value = ConvertExpr(ctx, p);
      dims.push_back(value);
    }

    if (x->dims.size() == x->lhs_sym->dims.size()) {
      // access to element
      if (x->dims.empty()) {
        // direct access
        auto* inst = new LoadInst(x->lhs_sym, x->lhs_sym->value,
                                  ConstValue::get(0), ctx->bb);
        return inst;
      }  // all levels except last level, emit GetElementPtr
      Value* val = x->lhs_sym->value;
      Inst* res = nullptr;
      for (u32 i = 0; i < x->dims.size(); i++) {
        int size = i + 1 < x->lhs_sym->dims.size()
                       ? x->lhs_sym->dims[i + 1]->result
                       : 1;
        if (i + 1 < x->dims.size()) {
          auto* inst =
              new GetElementPtrInst(x->lhs_sym, val, dims[i], size, ctx->bb);
          res = inst;
          val = inst;
        } else {
          auto* inst = new LoadInst(x->lhs_sym, val, dims[i], ctx->bb);
          res = inst;
        }
      }

      return res;
    }
    if (!x->dims.empty()) {
      // access to sub array
      // emit GetElementPtr for each level
      Value* val = x->lhs_sym->value;
      Inst* res = nullptr;
      for (u32 i = 0; i < x->dims.size(); i++) {
        int size = i + 1 < x->lhs_sym->dims.size()
                       ? x->lhs_sym->dims[i + 1]->result
                       : 1;
        auto* inst =
            new GetElementPtrInst(x->lhs_sym, val, dims[i], size, ctx->bb);
        res = inst;
        val = inst;
      }
      return res;
    }
    // access to array itself
    auto* inst = new GetElementPtrInst(x->lhs_sym, x->lhs_sym->value,
                                       ConstValue::get(0), 0, ctx->bb);
    return inst;
  }
  if (auto* x = dyn_cast<Call>(expr)) {
    // must evaluate args before calling
    std::vector<Value*> args;
    args.reserve(x->args.size());
    for (auto& p : x->args) {
      auto* value = ConvertExpr(ctx, p);
      args.push_back(value);
    }

    auto* inst = new CallInst(x->f->val, ctx->bb);

    // args
    inst->args.reserve(x->args.size());
    for (auto& value : args) {
      inst->args.emplace_back(value, inst);
    }
    return inst;
  }
  return nullptr;
}

void ConvertStmt(SSAContext* ctx, Stmt* stmt) {
  if (auto* x = dyn_cast<DeclStmt>(stmt)) {
    for (auto& decl : x->decls) {
      // alloca for local variable
      // we always put alloca to the first basic block of current function
      auto* inst = new AllocaInst(&decl, ctx->func->bb.head);
      decl.value = inst;

      if (decl.has_init) {
        if (decl.init.val1) {
          // store the init value in the allocated memory
          auto* init = ConvertExpr(ctx, decl.init.val1);
          new StoreInst(&decl, inst, init, ConstValue::get(0), ctx->bb);
        } else {
          // assign each element of flatten_init_list
          // count how many elements are zero, if there are more than 10 zero
          // then use memset to set all of them
          int num_zeros = 0;
          std::vector<Value*> values;
          values.reserve(decl.flatten_init_list.size());
          for (auto& i : decl.flatten_init_list) {
            auto* init = ConvertExpr(ctx, i);
            values.push_back(init);
            if (auto* x = dyn_cast<ConstValue>(init)) {
              if (x->imm == 0) {
                num_zeros++;
              }
            }
          }

          bool emit_memset = false;
          if (num_zeros > 10) {
            emit_memset = true;
            auto* call_inst =
                new CallInst(Func::builtin_function[8].val, ctx->bb);
            call_inst->args.reserve(3);
            // arr
            call_inst->args.emplace_back(inst, call_inst);
            // ch
            call_inst->args.emplace_back(ConstValue::get(0), call_inst);
            // count = num * 4
            call_inst->args.emplace_back(
                ConstValue::get(decl.dims[0]->result * 4), call_inst);
          }

          for (u32 i = 0; i < decl.flatten_init_list.size(); i++) {
            // skip safely
            if (auto* x = dyn_cast<ConstValue>(values[i])) {
              if (emit_memset && x->imm == 0) {
                continue;
              }
            }

            new StoreInst(&decl, inst, values[i], ConstValue::get(i), ctx->bb);
          }
        }
      }
    }
  } else if (auto* x = dyn_cast<Assign>(stmt)) {
    // first evaluate the dimensions
    std::vector<Value*> dims;
    dims.reserve(x->dims.size());
    for (auto& expr : x->dims) {
      auto* dim = ConvertExpr(ctx, expr);
      dims.push_back(dim);
    }

    auto* rhs = ConvertExpr(ctx, x->rhs);

    if (x->dims.empty()) {
      new StoreInst(x->lhs_sym, x->lhs_sym->value, rhs, ConstValue::get(0),
                    ctx->bb);
    } else {
      // all level except last level emit GetElementPtr, last level emit Store
      auto* last = x->lhs_sym->value;
      for (u64 i = 0; i < x->dims.size(); i++) {
        int size = i + 1 < x->lhs_sym->dims.size()
                       ? x->lhs_sym->dims[i + 1]->result
                       : 1;
        if (i != x->dims.size() - 1) {
          auto* inst =
              new GetElementPtrInst(x->lhs_sym, last, dims[i], size, ctx->bb);
          last = inst;
        } else {
          new StoreInst(x->lhs_sym, last, rhs, dims[i], ctx->bb);
        }
      }
    }
  } else if (auto* x = dyn_cast<If>(stmt)) {
    // 1. check `cond`
    // 2. branch to `then` or `else`
    // 3. jump to `end` at the end of `then` and `else`
    auto* cond = ConvertExpr(ctx, x->cond);
    auto* bb_then = new BasicBlock;
    auto* bb_else = new BasicBlock;
    auto* bb_end = new BasicBlock;
    ctx->func->bb.InsertAtEnd(bb_then);
    ctx->func->bb.InsertAtEnd(bb_else);
    ctx->func->bb.InsertAtEnd(bb_end);

    new BranchInst(cond, bb_then, bb_else, ctx->bb);

    // then branch
    ctx->bb = bb_then;
    ConvertStmt(ctx, x->on_true);
    // jump to the end basic block
    if (!ctx->bb->Valid()) {
      new JumpInst(bb_end, ctx->bb);
    }
    // else branch
    ctx->bb = bb_else;
    if (x->on_false) {
      ConvertStmt(ctx, x->on_false);
    }
    // jump to the end basic block
    if (!ctx->bb->Valid()) {
      new JumpInst(bb_end, ctx->bb);
    }

    // modify the context basic block
    ctx->bb = bb_end;
  } else if (auto* x = dyn_cast<While>(stmt)) {
    // 1. check `cond`
    // 2. branch to `loop` or `end`
    // 3. jump to `cond` at the end of `loop`
    auto* bb_cond = new BasicBlock;
    auto* bb_loop = new BasicBlock;
    auto* bb_end = new BasicBlock;

    ctx->func->bb.InsertAtEnd(bb_cond);
    ctx->func->bb.InsertAtEnd(bb_loop);

    new JumpInst(bb_cond, ctx->bb);
    ctx->bb = bb_cond;
    auto* cond = ConvertExpr(ctx, x->cond);
    new BranchInst(cond, bb_loop, bb_end, ctx->bb);

    ctx->bb = bb_loop;
    ctx->loop_stk.emplace_back(bb_cond, bb_end);
    ConvertStmt(ctx, x->body);
    ctx->loop_stk.pop_back();
    if (!ctx->bb->Valid()) {
      new JumpInst(bb_cond, ctx->bb);
    }

    ctx->func->bb.InsertAtEnd(bb_end);
    ctx->bb = bb_end;
  } else if (auto* x = dyn_cast<Block>(stmt)) {
    for (auto& stmt : x->stmts) {
      ConvertStmt(ctx, stmt);
      if (isa<Continue>(stmt) || isa<Break>(stmt) || isa<Return>(stmt)) {
        // omit the following stmts
        break;
      }
    }
  } else if (auto* x = dyn_cast<Return>(stmt)) {
    if (x->val) {
      auto* value = ConvertExpr(ctx, x->val);
      new ReturnInst(value, ctx->bb);
    } else {
      new ReturnInst(nullptr, ctx->bb);
    }
  } else if (auto* x = dyn_cast<ExprStmt>(stmt)) {
    if (x->val) {
      ConvertExpr(ctx, x->val);
    }
  } else if (isa<Continue>(stmt)) {
    new JumpInst(ctx->loop_stk.back().first, ctx->bb);
  } else if (isa<Break>(stmt)) {
    new JumpInst(ctx->loop_stk.back().second, ctx->bb);
  }
}

IrProgram* ConvertSSA(Program& program) {
  auto* ret = new IrProgram;

  // register all function, including builtin and user define function
  for (Func& builtin : Func::builtin_function) {
    auto* func = new IrFunc;
    func->builtin = true;
    func->func = &builtin;
    builtin.val = func;
    ret->func.InsertAtEnd(func);
  }
  for (auto& g : program.glob) {
    if (Func* f = std::get_if<0>(&g)) {
      auto* func = new IrFunc;
      func->builtin = false;
      func->func = f;
      f->val = func;
      ret->func.InsertAtEnd(func);
    }
  }

  for (auto& g : program.glob) {
    if (Func* f = std::get_if<0>(&g)) {
      IrFunc* func = f->val;
      auto* entry_basic_block = new BasicBlock;
      func->bb.InsertAtEnd(entry_basic_block);

      // allocate space for function parameter which is not array
      for (auto& decl : f->params) {
        if (decl.dims.empty()) {
          auto* inst = new AllocaInst(&decl, entry_basic_block);
          decl.value = inst;
          // copy the actual data into the parameter
          new StoreInst(&decl, inst, new ParamRef(&decl), ConstValue::get(0),
                        entry_basic_block);
        } else {
          decl.value = new ParamRef(&decl);
        }
      }

      SSAContext ctx = {ret, func, entry_basic_block};
      // first block in ssa context is entry block
      for (auto& stmt : f->body.stmts) {
        ConvertStmt(&ctx, stmt);
      }

      // add extra return instruction for invalid basic block
      if (!ctx.bb->Valid()) {
        if (func->func->is_int) {
          new ReturnInst(ConstValue::get(0), ctx.bb);
        } else {
          new ReturnInst(nullptr, ctx.bb);
        }
      }

      for (BasicBlock* bb = func->bb.head; bb; bb = bb->next) {
        bb->pred.clear();
      }
      for (BasicBlock* bb = func->bb.head; bb; bb = bb->next) {
        for (BasicBlock* x : bb->Succ()) {
          if (x) x->pred.push_back(bb);
        }
      }
    } else {
      Decl* d = std::get_if<1>(&g);
      ret->glob_decl.push_back(d);
      d->value = new GlobalRef(d);
    }
  }
  return ret;
}
