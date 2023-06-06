#pragma once

#include <cstdint>
#include <string_view>
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
  i32 result;  // 保存解析出来的结果，typeck阶段计算它，后续用它
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
  Decl* lhs_sym;  // typeck前是nullptr，若typeck成功则非空
};

struct IntConst : Expr {
  DEFINE_CLASSOF(Expr, p->tag == Tag::IntConst);

  i32 val;
};

struct Call : Expr {
  DEFINE_CLASSOF(Expr, p->tag == Tag::Call);

  std::string func;
  std::vector<Expr*> args;
  Func* f = nullptr;  // typeck前是nullptr，若typeck成功则非空

  // do some simple preprocess in constructor
  explicit Call(std::string_view func, std::vector<Expr*> args)
      : Expr{Tag::Call, 0}, func(func), args(args) {}
};

struct InitList {
  // val1为nullptr时val2有效，逻辑上相当于std::variant<Expr *,
  // std::vector<InitList>>，但是stl这一套实在是不好用
  Expr* val1;  // nullable
  std::vector<InitList> val2;
};

struct Value;

struct Decl {
  bool is_const;
  bool is_glob;
  bool has_init;  // 配合init使用
  std::string name;
  // 基本类型总是int，所以不记录，只记录数组维度
  // dims[0]可能是nullptr，当且仅当Decl用于Func::params，且参数形如int
  // a[][10]时；其他情况下每个元素都非空
  // 经过typeck后，每个维度保存的result是包括它在内右边所有维度的乘积，例如int[2][3][4]就是{24,
  // 12, 4}
  std::vector<Expr*> dims;
  InitList
      init;  // 配合has_init，逻辑上相当于std::optional<InitList>，但是stl这一套实在是不好用
  std::vector<Expr*>
      FlattenInitList;  // parse完后为空，typeck阶段填充，是完全展开+补0后的init

  // ast->ir阶段赋值，每个Decl拥有属于自己的Value，Value
  // *的地址相等等价于指向同一个的变量
  // 一开始全局变量：GlobalRef，参数中的数组：ParamRef，参数中的int/局部变量：AllocaInst
  // 经过mem2reg后，参数和局部变量中的int将不再需要这个AllocaInst
  Value* value;

  bool is_param_array() const { return !dims.empty() && dims[0] == nullptr; }
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
  Decl* lhs_sym;  // typeck前是nullptr，若typeck成功则非空
};

struct ExprStmt : Stmt {
  DEFINE_CLASSOF(Stmt, p->tag == Stmt::ExprStmt);

  Expr* val;  // nullable，为空时就是一条分号
};

struct DeclStmt : Stmt {
  DEFINE_CLASSOF(Stmt, p->tag == Stmt::DeclStmt);

  std::vector<Decl> decls;  // 一条语句可以定义多个变量
};

struct Block : Stmt {
  DEFINE_CLASSOF(Stmt, p->tag == Stmt::Block);

  std::vector<Stmt*> stmts;
};

struct If : Stmt {
  DEFINE_CLASSOF(Stmt, p->tag == Stmt::If);

  Expr* cond;
  Stmt* on_true;
  Stmt* on_false;  // nullable
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

  Expr* val;  // nullable
};

struct IrFunc;

struct Func {
  // return type can only be void or int
  bool IsInt;
  std::string name;
  // 只是用Decl来复用一下代码，其实不能算是Decl，is_const / is_glob /
  // has_init总是false
  std::vector<Decl> params;
  Block body;

  // ast->ir阶段赋值
  IrFunc* val;

  // BUILTIN[8]是memset，这个下标在ssa.cpp会用到，修改时需要一并修改
  static Func BUILTIN[9];
};

inline Func Func::BUILTIN[9] = {
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
