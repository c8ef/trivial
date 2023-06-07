#pragma once

#include <array>
#include <cassert>
#include <cstdint>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "casting.hpp"
#include "common.hpp"

struct Func;
struct Decl;

struct Inst;
struct IrFunc;
struct Use;
struct MemPhiInst;

struct Value {
  // value is used by ...
  IntrusiveList<Use> uses;
  // tag
  enum class Tag {
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
    Branch,
    Jump,
    Return,
    // Control flow
    GetElementPtr,
    Load,
    Store,
    // Memory
    Call,
    Alloca,
    Phi,
    MemOp,
    MemPhi,
    // 虚拟的MemPhi指令，保证不出现在指令序列中，只出现在BasicBlock::mem_phis中
    Const,
    Global,
    Param,
    Undef,
    // Const ~ Undef: Reference
  } tag;

  explicit Value(Tag tag) : tag(tag) {}

  void AddUse(Use* u) { uses.InsertAtEnd(u); }
  void KillUse(Use* u) { uses.Remove(u); }

  // 将对自身所有的使用替换成对v的使用
  inline void replaceAllUseWith(Value* v);
  // 调用deleteValue语义上相当于delete掉它，但是按照现在的实现不能直接delete它
  void deleteValue();
};

struct Use {
  DEFINE_LIST(Use)

  Value* value;
  Inst* user;

  // 这个构造函数没有初始化prev和next，这没有关系
  // 因为prev和next永远不会从一个Use开始被主动使用，而是在遍历Use链表的时候用到
  // 而既然这个Use已经被加入了一个链表，它的prev和next也就已经被赋值了
  Use(Value* v, Inst* u) : value(v), user(u) {
    if (v) v->AddUse(this);
  }

  // 没有必要定义移动构造函数/拷贝运算符，语义没有区别
  // 一般不会用到它们，只在类似vector内部构造临时变量又析构的场景中用到
  Use(const Use& rhs) : value(rhs.value), user(rhs.user) {
    if (value) value->AddUse(this);
  }
  Use& operator=(const Use& rhs) {
    if (this != &rhs) {
      assert(user == rhs.user);
      set(rhs.value);
    }
    return *this;
  }

  // 注意不要写.value = xxx, 而是用.set(xxx), 因为需要记录被use的关系
  void set(Value* v) {
    if (value) value->KillUse(this);
    value = v;
    if (v) v->AddUse(this);
  }

  ~Use() {
    if (value) value->KillUse(this);
  }
};

void Value::replaceAllUseWith(Value* v) {
  // head->set会将head从链表中移除
  while (uses.head) uses.head->set(v);
}

struct IrProgram {
  IntrusiveList<IrFunc> func;
  std::vector<Decl*> glob_decl;
};

std::ostream& operator<<(std::ostream& os, const IrProgram& dt);

struct BasicBlock {
  DEFINE_LIST(BasicBlock)

  std::vector<BasicBlock*> pred;
  BasicBlock* idom;
  std::unordered_set<BasicBlock*> dom_by;  // 支配它的节点集
  std::vector<BasicBlock*> doms;           // 它支配的节点集
  u32 dom_level;                           // dom树中的深度，根深度为0
  // 各种算法中用到，标记是否访问过，算法开头应把所有vis置false(调用IrFunc::clear_all_vis)
  bool vis;
  IntrusiveList<Inst> insts;
  IntrusiveList<Inst> mem_phis;  // 元素都是MemPhiInst

  std::array<BasicBlock*, 2> succ();
  std::array<BasicBlock**, 2> succ_ref();  // 想修改succ时使用
  bool valid();
};

struct IrFunc {
  DEFINE_LIST(IrFunc)

  Func* func;
  IntrusiveList<BasicBlock> bb;
  // functions called by this function
  std::set<IrFunc*> callee_func;
  // functions calling this function
  std::set<IrFunc*> caller_func;
  bool builtin;
  bool load_global;
  // has_side_effect:
  // 修改了全局变量/传入的数组参数，或者调用了has_side_effect的函数 no side
  // effect函数的没有user的调用可以删除
  bool has_side_effect;
  bool can_inline;

  // pure函数的参数相同的调用可以删除
  bool pure() const { return !(load_global || has_side_effect); }

  // 将所有bb的vis置false
  void clear_all_vis() {
    for (BasicBlock* b = bb.head; b; b = b->next) b->vis = false;
  }
};

struct ConstValue : Value {
  DEFINE_CLASSOF(Value, p->tag == Tag::Const);
  const i32 imm;

  static std::unordered_map<i32, ConstValue*> POOL;

  static ConstValue* get(i32 imm) {
    auto [it, inserted] = POOL.insert({imm, nullptr});
    if (inserted) it->second = new ConstValue(imm);
    return it->second;
  }

 private:
  // use ConstValue::get instead
  explicit ConstValue(i32 imm) : Value(Tag::Const), imm(imm) {}
};

struct GlobalRef : Value {
  DEFINE_CLASSOF(Value, p->tag == Tag::Global);
  Decl* decl;

  explicit GlobalRef(Decl* decl) : Value(Tag::Global), decl(decl) {}
};

struct ParamRef : Value {
  DEFINE_CLASSOF(Value, p->tag == Tag::Param);
  Decl* decl;

  explicit ParamRef(Decl* decl) : Value(Tag::Param), decl(decl) {}
};

struct UndefValue : Value {
  DEFINE_CLASSOF(Value, p->tag == Tag::Undef);

  UndefValue() : Value(Tag::Undef) {}
};

struct Inst : Value {
  DEFINE_CLASSOF(Value, Tag::Add <= p->tag && p->tag <= Tag::MemPhi);
  DEFINE_LIST(Inst)
  // basic block
  BasicBlock* bb;

  // insert this inst before `InsertBefore`
  Inst(Tag tag, Inst* InsertBefore) : Value(tag), bb(InsertBefore->bb) {
    bb->insts.InsertBefore(this, InsertBefore);
  }

  // insert this inst at the end of `InsertAtEnd`
  Inst(Tag tag, BasicBlock* InsertAtEnd) : Value(tag), bb(InsertAtEnd) {
    bb->insts.InsertAtEnd(this);
  }

  // 只初始化tag，没有加入到链表中，调用者手动加入
  Inst(Tag tag) : Value(tag) {}

  // 返回的指针对是一个左闭右开区间，表示这条指令的所有操作数，.value可能为空
  std::pair<Use*, Use*> operands();

  bool has_side_effect();
};

struct BinaryInst : Inst {
  DEFINE_CLASSOF(Value, Tag::Add <= p->tag && p->tag <= Tag::Or);
  // operands
  // loop unroll pass里用到了lhs和rhs的摆放顺序，不要随便修改
  Use lhs;
  Use rhs;

  BinaryInst(Tag tag, Value* lhs, Value* rhs, BasicBlock* insert_at_end)
      : Inst(tag, insert_at_end), lhs(lhs, this), rhs(rhs, this) {}

  BinaryInst(Tag tag, Value* lhs, Value* rhs, Inst* insert_before)
      : Inst(tag, insert_before), lhs(lhs, this), rhs(rhs, this) {}

  bool rhsCanBeImm() {
    // Add, Sub, Rsb, Mul, Div, Mod, Lt, Le, Ge, Gt, Eq, Ne, And, Or
    return (tag >= Tag::Add && tag <= Tag::Rsb) ||
           (tag >= Tag::Lt && tag <= Tag::Or);
  }

  constexpr static const char* kLLVMOps[14] = {
      /* Add = */ "add",
      /* Sub = */ "sub",
      /* Rsb = */ nullptr,
      /* Mul = */ "mul",
      /* Div = */ "sdiv",
      /* Mod = */ "srem",
      /* Lt = */ "icmp slt",
      /* Le = */ "icmp sle",
      /* Ge = */ "icmp sge",
      /* Gt = */ "icmp sgt",
      /* Eq = */ "icmp eq",
      /* Ne = */ "icmp ne",
      /* And = */ "and",
      /* Or = */ "or",
  };

  constexpr static std::pair<Tag, Tag> kSwappableOperators[11] = {
      {Tag::Add, Tag::Add}, {Tag::Sub, Tag::Rsb}, {Tag::Mul, Tag::Mul},
      {Tag::Lt, Tag::Gt},   {Tag::Le, Tag::Ge},   {Tag::Gt, Tag::Lt},
      {Tag::Ge, Tag::Le},   {Tag::Eq, Tag::Eq},   {Tag::Ne, Tag::Ne},
      {Tag::And, Tag::And}, {Tag::Or, Tag::Or},
  };

  bool swapOperand() {
    for (auto [before, after] : kSwappableOperators) {
      if (tag == before) {
        // note:
        // Use是被pin在内存中的，不能直接swap它们。如果未来希望这样做，需要实现配套的设施，基本上就是把下面的逻辑在构造函数/拷贝运算符中实现
        tag = after;
        Value *l = lhs.value, *r = rhs.value;
        l->KillUse(&lhs);
        r->KillUse(&rhs);
        l->AddUse(&rhs);
        r->AddUse(&lhs);
        std::swap(lhs.value, rhs.value);
        return true;
      }
    }
    return false;
  }

  Value* optimizedValue() {
    // imm on rhs
    if (auto r = dyn_cast<ConstValue>(rhs.value)) {
      switch (tag) {
        case Tag::Add:
        case Tag::Sub:
          return r->imm == 0 ? lhs.value : nullptr;  // ADD or SUB 0
        case Tag::Mul:
          if (r->imm == 0) return ConstValue::get(0);  // MUL 0
          [[fallthrough]];
        case Tag::Div:
          if (r->imm == 1) return lhs.value;  // MUL or DIV 1
        case Tag::Mod:
          return r->imm == 1 ? ConstValue::get(0) : nullptr;  // MOD 1
        case Tag::And:
          if (r->imm == 0) return ConstValue::get(0);  // AND 0
          return r->imm == 1 ? lhs.value : nullptr;    // AND 1
        case Tag::Or:
          if (r->imm == 1) return ConstValue::get(1);  // OR 1
          return r->imm == 0 ? lhs.value : nullptr;    // OR 0
        default:
          return nullptr;
      }
    }
    return nullptr;
  }
};

struct BranchInst : Inst {
  DEFINE_CLASSOF(Value, p->tag == Tag::Branch);
  Use cond;
  // true
  BasicBlock* left;
  // false
  BasicBlock* right;

  BranchInst(Value* cond, BasicBlock* left, BasicBlock* right,
             BasicBlock* insert_at_end)
      : Inst(Tag::Branch, insert_at_end),
        cond(cond, this),
        left(left),
        right(right) {}
};

struct JumpInst : Inst {
  DEFINE_CLASSOF(Value, p->tag == Tag::Jump);
  BasicBlock* next;

  JumpInst(BasicBlock* next, BasicBlock* insert_at_end)
      : Inst(Tag::Jump, insert_at_end), next(next) {}
};

struct ReturnInst : Inst {
  DEFINE_CLASSOF(Value, p->tag == Tag::Return);
  Use ret;

  ReturnInst(Value* ret, BasicBlock* insert_at_end)
      : Inst(Tag::Return, insert_at_end), ret(ret, this) {}
};

struct AccessInst : Inst {
  DEFINE_CLASSOF(Value, p->tag == Tag::GetElementPtr || p->tag == Tag::Load ||
                            p->tag == Tag::Store);
  Decl* lhs_sym;
  Use arr;
  Use index;
  AccessInst(Inst::Tag tag, Decl* lhs_sym, Value* arr, Value* index,
             BasicBlock* insert_at_end)
      : Inst(tag, insert_at_end),
        lhs_sym(lhs_sym),
        arr(arr, this),
        index(index, this) {}
};

struct GetElementPtrInst : AccessInst {
  DEFINE_CLASSOF(Value, p->tag == Tag::GetElementPtr);
  int multiplier;
  GetElementPtrInst(Decl* lhs_sym, Value* arr, Value* index, int multiplier,
                    BasicBlock* insert_at_end)
      : AccessInst(Tag::GetElementPtr, lhs_sym, arr, index, insert_at_end),
        multiplier(multiplier) {}
};

struct LoadInst : AccessInst {
  DEFINE_CLASSOF(Value, p->tag == Tag::Load);
  Use mem_token;  // 由memdep pass计算
  LoadInst(Decl* lhs_sym, Value* arr, Value* index, BasicBlock* insert_at_end)
      : AccessInst(Tag::Load, lhs_sym, arr, index, insert_at_end),
        mem_token(nullptr, this) {}
};

struct StoreInst : AccessInst {
  DEFINE_CLASSOF(Value, p->tag == Tag::Store);
  Use data;
  StoreInst(Decl* lhs_sym, Value* arr, Value* data, Value* index,
            BasicBlock* insert_at_end)
      : AccessInst(Tag::Store, lhs_sym, arr, index, insert_at_end),
        data(data, this) {}
};

struct CallInst : Inst {
  DEFINE_CLASSOF(Value, p->tag == Tag::Call);
  IrFunc* func;
  std::vector<Use> args;
  CallInst(IrFunc* func, BasicBlock* insert_at_end)
      : Inst(Tag::Call, insert_at_end), func(func) {}
};

struct AllocaInst : Inst {
  DEFINE_CLASSOF(Value, p->tag == Tag::Alloca);

  Decl* sym;
  AllocaInst(Decl* sym, BasicBlock* insert_before)
      : Inst(Tag::Alloca, insert_before), sym(sym) {}
};

struct PhiInst : Inst {
  DEFINE_CLASSOF(Value, p->tag == Tag::Phi);
  std::vector<Use> incoming_values;
  std::vector<BasicBlock*>& incoming_bbs() { return bb->pred; }

  explicit PhiInst(BasicBlock* insert_at_front) : Inst(Tag::Phi) {
    bb = insert_at_front;
    bb->insts.InsertAtBegin(this);
    u32 n = incoming_bbs().size();
    incoming_values.reserve(n);
    for (u32 i = 0; i < n; ++i) {
      // 在new
      // PhiInst的时候还不知道它用到的value是什么，先填nullptr，后面再用Use::set填上
      incoming_values.emplace_back(nullptr, this);
    }
  }

  explicit PhiInst(Inst* insert_before) : Inst(Tag::Phi, insert_before) {
    u32 n = incoming_bbs().size();
    incoming_values.reserve(n);
    for (u32 i = 0; i < n; ++i) {
      incoming_values.emplace_back(nullptr, this);
    }
  }
};

struct MemOpInst : Inst {
  DEFINE_CLASSOF(Value, p->tag == Tag::MemOp);
  Use mem_token;
  LoadInst* load;
  MemOpInst(LoadInst* load, Inst* insert_before)
      : Inst(Tag::MemOp, insert_before), mem_token(nullptr, this), load(load) {}
};

// 它的前几个字段和PhiInst是兼容的，所以可以当成PhiInst用(也许理论上有隐患，但是实际上应该没有问题)
// 我不希望让它继承PhiInst，这也许会影响以前的一些对PhiInst的使用
struct MemPhiInst : Inst {
  DEFINE_CLASSOF(Value, p->tag == Tag::MemPhi);
  std::vector<Use> incoming_values;
  std::vector<BasicBlock*>& incoming_bbs() { return bb->pred; }

  // load依赖store和store依赖load两种依赖用到的MemPhiInst不一样
  // 前者的load_or_arr来自于load的数组地址，类型是Decl
  // *，后者的load_or_arr来自于LoadInst
  void* load_or_arr;

  explicit MemPhiInst(void* load_or_arr, BasicBlock* insert_at_front)
      : Inst(Tag::MemPhi), load_or_arr(load_or_arr) {
    bb = insert_at_front;
    bb->mem_phis.InsertAtBegin(this);
    u32 n = incoming_bbs().size();
    incoming_values.reserve(n);
    for (u32 i = 0; i < n; ++i) {
      incoming_values.emplace_back(nullptr, this);
    }
  }
};
