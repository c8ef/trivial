#pragma once

#include <array>
#include <cstdint>
#include <map>
#include <string_view>
#include <unordered_set>
#include <vector>

#include "ast.hpp"
#include "casting.hpp"
#include "common.hpp"
#include "ilist.hpp"
#include "typeck.hpp"

struct Inst;
struct IrFunc;
struct Use;

struct Value {
  // value is used by ...
  ilist<Use> uses;
  // tag
  enum Tag {
    Add,
    Sub,
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
    Or,  // Binary
    Neg,
    Not,
    Mv,  // Unary
    Branch,
    Jump,
    Return,  // Control flow
    Load,
    Store,  // Memory
    Call,
    Alloca,
    Phi,
    Const,
    Global,
    Param,
    Undef,  // Const ~ Undef: Reference
  } tag;

  Value(Tag tag) : tag(tag) {}

  void addUse(Use *u) { uses.insertAtEnd(u); }
  void killUse(Use *u) { uses.remove(u); }

  // 将对自身所有的使用替换成对v的使用
  inline void replaceAllUseWith(Value *v);
};

struct Use {
  DEFINE_ILIST(Use)

  Value *value;
  Inst *user;

  // 这个构造函数没有初始化prev和next，这没有关系
  // 因为prev和next永远不会从一个Use开始被主动使用，而是在遍历Use链表的时候用到
  // 而既然这个Use已经被加入了一个链表，它的prev和next也就已经被赋值了
  Use(Value *v, Inst *u) : value(v), user(u) {
    if (v) v->addUse(this);
  }

  void set(Value *v) {
    if (value) value->killUse(this);
    value = v;
    if (v) v->addUse(this);
  }

  ~Use() {
    if (value) value->killUse(this);
  }
};

void Value::replaceAllUseWith(Value *v) {
  // head->set会将head从链表中移除
  while (uses.head) uses.head->set(v);
}

struct IrProgram {
  ilist<IrFunc> func;
  std::vector<Decl *> glob_decl;

  IrFunc *findFunc(Func *func);
  friend std::ostream &operator<<(std::ostream &os, const IrProgram &dt);
};

std::ostream &operator<<(std::ostream &os, const IrProgram &dt);

struct BasicBlock {
  DEFINE_ILIST(BasicBlock)
  std::vector<BasicBlock *> pred;
  std::unordered_set<BasicBlock *> dom;  // 支配它的节点集
  BasicBlock *idom;                      // 直接支配它的节点
  ilist<Inst> insts;

  inline std::array<BasicBlock *, 2> succ();
  inline bool valid();
};

struct IrFunc {
  DEFINE_ILIST(IrFunc)
  Func *func;
  ilist<BasicBlock> bb;
  // mapping from decl to its value in this function
  std::map<Decl *, Value *> decls;
};

struct ConstValue : Value {
  DEFINE_CLASSOF(Value, p->tag == Const);
  i32 imm;

  ConstValue(i32 imm) : Value(Const), imm(imm) {}
};

struct GlobalRef : Value {
  DEFINE_CLASSOF(Value, p->tag == Global);
  Decl *decl;

  GlobalRef(Decl *decl) : Value(Global), decl(decl) {}
};

struct ParamRef : Value {
  DEFINE_CLASSOF(Value, p->tag == Param);
  Decl *decl;

  ParamRef(Decl *decl) : Value(Param), decl(decl) {}
};

struct UndefValue : Value {
  DEFINE_CLASSOF(Value, p->tag == Undef);

  UndefValue() : Value(Undef) {}
  // 这是一个全局可变变量，不过反正也不涉及多线程，不会有冲突
  static UndefValue INSTANCE;
};

struct Inst : Value {
  DEFINE_CLASSOF(Value, Add <= p->tag && p->tag <= Alloca);
  // instruction linked list
  DEFINE_ILIST(Inst)
  // basic block
  BasicBlock *bb;

  // insert this inst before `insertBefore`
  Inst(Tag tag, Inst *insertBefore) : Value(tag), bb(insertBefore->bb) {
    bb->insts.insertBefore(this, insertBefore);
  }

  // insert this inst at the end of `insertAtEnd`
  Inst(Tag tag, BasicBlock *insertAtEnd) : Value(tag), bb(insertAtEnd) {
    bb->insts.insertAtEnd(this);
  }
};

struct BinaryInst : Inst {
  DEFINE_CLASSOF(Value, Add <= p->tag && p->tag <= Or);
  // operands
  Use lhs;
  Use rhs;

  BinaryInst(Tag tag, Value *lhs, Value *rhs, Inst *insertBefore)
      : Inst(tag, insertBefore), lhs(lhs, this), rhs(rhs, this) {}
  BinaryInst(Tag tag, Value *lhs, Value *rhs, BasicBlock *insertAtEnd)
      : Inst(tag, insertAtEnd), lhs(lhs, this), rhs(rhs, this) {}
};

struct UnaryInst : Inst {
  DEFINE_CLASSOF(Value, Neg <= p->tag && p->tag <= Mv);
  // operands
  Use rhs;
  UnaryInst(Tag tag, Value *rhs, BasicBlock *insertAtEnd) : Inst(tag, insertAtEnd), rhs(rhs, this) {}
};

struct LoadInst : Inst {
  DEFINE_CLASSOF(Value, p->tag == Load);
  Decl *lhs_sym;
  Use arr;
  std::vector<Use> dims;
  LoadInst(Decl *lhs_sym, Value *arr, BasicBlock *insertAtEnd)
      : Inst(Load, insertAtEnd), lhs_sym(lhs_sym), arr(arr, this) {}
};

struct StoreInst : Inst {
  DEFINE_CLASSOF(Value, p->tag == Store);
  Decl *lhs_sym;
  Use arr;
  std::vector<Use> dims;
  Use data;

  StoreInst(Decl *lhs_sym, Value *arr, Value *data, BasicBlock *insertAtEnd)
      : Inst(Store, insertAtEnd), lhs_sym(lhs_sym), arr(arr, this), data(data, this) {}
};

struct CallInst : Inst {
  DEFINE_CLASSOF(Value, p->tag == Call);
  // FIXME: IrFunc and Func 是什么关系？
  Func *func;
  std::vector<Use> args;
  CallInst(Func *func, BasicBlock *insertAtEnd) : Inst(Call, insertAtEnd), func(func) {}
};

struct BranchInst : Inst {
  DEFINE_CLASSOF(Value, p->tag == Branch);
  Use cond;
  // true
  BasicBlock *left;
  // false
  BasicBlock *right;

  BranchInst(Value *cond, BasicBlock *left, BasicBlock *right, BasicBlock *insertAtEnd)
      : Inst(Branch, insertAtEnd), cond(cond, this), left(left), right(right) {}
};

struct JumpInst : Inst {
  DEFINE_CLASSOF(Value, p->tag == Jump);
  BasicBlock *next;

  JumpInst(BasicBlock *next, BasicBlock *insertAtEnd) : Inst(Jump, insertAtEnd), next(next) {}
};

struct ReturnInst : Inst {
  DEFINE_CLASSOF(Value, p->tag == Return);
  Use ret;

  ReturnInst(Value *ret, BasicBlock *insertAtEnd) : Inst(Return, insertAtEnd), ret(ret, this) {}
};

struct AllocaInst : Inst {
  DEFINE_CLASSOF(Value, p->tag == Alloca);

  Decl *sym;
  AllocaInst(Decl *sym, BasicBlock *insertBefore) : Inst(Alloca, insertBefore), sym(sym) {}
};

struct PhiInst : Inst {
  DEFINE_CLASSOF(Value, p->tag == Phi);

  // incoming_values.size() == incoming_bbs.size()
  std::vector<Use> incoming_values;
  std::vector<BasicBlock *> *incoming_bbs;  // todo: 指向拥有它的bb的pred，这是正确的吗？

  explicit PhiInst(BasicBlock *insertAtFront) : Inst(Phi, insertAtFront->insts.head), incoming_bbs(&insertAtFront->pred) {
    incoming_values.reserve(insertAtFront->pred.size());
    for (u32 i = 0; i < insertAtFront->pred.size(); ++i) {
      // 在new PhiInst的时候还不知道它用到的value是什么，先填nullptr，后面再用Use::set填上
      incoming_values.emplace_back(nullptr, this);
    }
  }
};

std::array<BasicBlock *, 2> BasicBlock::succ() {
  Inst *end = insts.tail;  // 必须非空
  if (auto x = dyn_cast<BranchInst>(end))
    return {x->left, x->right};
  else if (auto x = dyn_cast<JumpInst>(end))
    return {x->next, nullptr};
  else if (auto x = dyn_cast<ReturnInst>(end))
    return {nullptr, nullptr};
  else
    UNREACHABLE();
}

bool BasicBlock::valid() {
  Inst *end = insts.tail;
  return end && (isa<BranchInst>(end) || isa<JumpInst>(end) || isa<ReturnInst>(end));
}