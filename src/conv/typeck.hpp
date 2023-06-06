#pragma once

#include "structure/ast.hpp"

#define ERR(...) ERR_EXIT(TypeCheckError, __VA_ARGS__)

// use 2 LSB differentiate Func and Decl
struct Symbol {
  size_t p;

  static Symbol MakeFunc(Func* f) {
    return Symbol{reinterpret_cast<size_t>(f) | 1U};
  }

  static Symbol MakeDecl(Decl* d) {
    return Symbol{reinterpret_cast<size_t>(d) | 2U};
  }

  [[nodiscard]] Func* AsFunc() const {
    return (p & 1U) != 0U ? reinterpret_cast<Func*>(p ^ 1U) : nullptr;
  }

  [[nodiscard]] Decl* AsDecl() const {
    return (p & 2U) != 0U ? reinterpret_cast<Decl*>(p ^ 2U) : nullptr;
  }
};

struct Env {
  Func* LookupFunc(std::string name);
  Decl* LookupDecl(std::string name);
  void CheckFunc(Func* f);
  void CheckDecl(Decl& d);
  void CheckStmt(Stmt* s);
  std::pair<Expr**, Expr**> CheckExpr(Expr* e);
  void Eval(Expr* e);
  void FlattenInitList(std::vector<InitList>& src, Expr** dims, Expr** dims_end,
                       bool need_eval, std::vector<Expr*>& dst);

  static bool IsInt(std::pair<Expr**, Expr**> t);

  // global symbols
  std::unordered_map<std::string, Symbol> glob;
  // stacks of local decls
  std::vector<std::unordered_map<std::string, Decl*>> local_stk;
  Func* cur_func = nullptr;
  u32 loop_cnt = 0;
};

void TypeCheck(Program& p);