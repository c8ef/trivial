#pragma once

#include <cstdint>
#include <string_view>
#include <utility>
#include <variant>
#include <vector>

#include "common.hpp"

struct Decl;
struct Func;

struct Expr {
  enum Tag {
    // Binary Operator Start
    Add,
    Sub,
    Rsb,
    Mul,
    Div,
    Mod,
    Lt,
    Le,
    Ge,
    Gt,
    Eq,
    Ne,
    And,
    Or,
    // Binary Operator End
    Call,
    Index,
    IntConst
  } tag;
  i32 result;  // store the calculated result during type check
};

struct Binary : Expr {
  DEFINE_CLASSOF(Expr, Tag::Add <= p->tag && p->tag <= Tag::Or);

  Expr* lhs;
  Expr* rhs;
};

struct Index : Expr {
  DEFINE_CLASSOF(Expr, p->tag == Tag::Index);

  std::string name;
  std::vector<Expr*> dims;
  Decl* lhs_sym;
};

struct IntConst : Expr {
  DEFINE_CLASSOF(Expr, p->tag == Tag::IntConst);

  i32 val;
};

struct Call : Expr {
  DEFINE_CLASSOF(Expr, p->tag == Tag::Call);

  std::string func;
  std::vector<Expr*> args;
  Func* f = nullptr;

  // do some simple preprocess in constructor
  explicit Call(std::string_view func, std::vector<Expr*> args)
      : Expr{Tag::Call, 0}, func(func), args(std::move(args)) {}
};

struct InitList {
  // when val1 == nullptr, val2 is valid
  // when val2 is empty, val1 = ParseExpr()
  Expr* val1;
  std::vector<InitList> val2;
};

struct Value;

struct Decl {
  bool is_const;
  bool is_glob;
  bool has_init;
  std::string name;

  // when decl is used as function parameter, dims[0] can be nullptr
  // after type check, int[2][3][4] -> {24, 12, 4}
  std::vector<Expr*> dims;
  InitList init;
  // fill in the type check phase
  std::vector<Expr*> flatten_init_list;

  // fill in the ConvertSSA phase
  // global variable: GlobalRef
  // array in parameter: ParamRef
  // int in parameter/local variable: AllocaInst
  Value* value;

  [[nodiscard]] bool IsParamArray() const { return !dims.empty() && !dims[0]; }
};

struct Stmt {
  enum {
    Assign,
    ExprStmt,
    DeclStmt,
    Block,
    If,
    While,
    Break,
    Continue,
    Return
  } tag;
};

struct Assign : Stmt {
  DEFINE_CLASSOF(Stmt, p->tag == Stmt::Assign);

  std::string ident;
  std::vector<Expr*> dims;
  Expr* rhs;
  Decl* lhs_sym;
};

struct ExprStmt : Stmt {
  DEFINE_CLASSOF(Stmt, p->tag == Stmt::ExprStmt);

  Expr* val;
};

struct DeclStmt : Stmt {
  DEFINE_CLASSOF(Stmt, p->tag == Stmt::DeclStmt);

  std::vector<Decl> decls;
};

struct Block : Stmt {
  DEFINE_CLASSOF(Stmt, p->tag == Stmt::Block);

  std::vector<Stmt*> stmts;
};

struct If : Stmt {
  DEFINE_CLASSOF(Stmt, p->tag == Stmt::If);

  Expr* cond;
  Stmt* on_true;
  Stmt* on_false;
};

struct While : Stmt {
  DEFINE_CLASSOF(Stmt, p->tag == Stmt::While);

  Expr* cond;
  Stmt* body;
};

struct Break : Stmt {
  DEFINE_CLASSOF(Stmt, p->tag == Stmt::Break);
};

struct Continue : Stmt {
  DEFINE_CLASSOF(Stmt, p->tag == Stmt::Continue);
};

struct Return : Stmt {
  DEFINE_CLASSOF(Stmt, p->tag == Stmt::Return);

  Expr* val;
};

struct IrFunc;

struct Func {
  // return type can only be void or int
  bool is_int;
  std::string name;
  // is_const | is_glob | has_init always false
  std::vector<Decl> params;
  Block body;

  // get value in ConvertSSA
  IrFunc* val;

  // builtin_function[8] = memset, used in ConvertStmt
  static Func builtin_function[9];
};

inline Func Func::builtin_function[9] = {
    Func{true, "getint"},
    Func{true, "getch"},
    Func{true, "getarray", {Decl{false, false, false, "a", {nullptr}}}},
    Func{false, "putint", {Decl{false, false, false, "n"}}},
    Func{false, "putch", {Decl{false, false, false, "c"}}},
    Func{false,
         "putarray",
         {Decl{false, false, false, "n"},
          Decl{false, false, false, "a", {nullptr}}}},
    Func{false, "starttime"},
    Func{false, "stoptime"},
    Func{false,
         "memset",
         {Decl{false, false, false, "arr", {nullptr}},
          Decl{false, false, false, "num"},
          Decl{false, false, false, "count"}}}};

struct Program {
  std::vector<std::variant<Func, Decl>> glob;
};
