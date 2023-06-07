#pragma once

#include <cassert>
#include <iomanip>
#include <iostream>
#include <set>

#include "common.hpp"
#include "structure/ast.hpp"
#include "structure/ir.hpp"

struct MachineFunc;
struct MachineBB;
struct MachineInst;
struct MachineOperand;

// ref: https://en.wikipedia.org/wiki/Calling_convention#ARM_(A32)
enum class ArmReg {
  // args and return value (caller saved)
  r0,
  r1,
  r2,
  r3,
  // local variables (callee saved)
  r4,
  r5,
  r6,
  r7,
  r8,
  r9,
  r10,
  r11,
  // special purposes
  r12,
  r13,
  r14,
  r15,
  // some aliases
  fp = r11,  // frame pointer (omitted), allocatable
  ip = r12,  // ipc scratch register, used in some instructions (caller saved)
  sp = r13,  // stack pointer
  lr = r14,  // link register (caller saved)
  pc = r15,  // program counter
};

enum class ArmCond { Any, Eq, Ne, Ge, Gt, Le, Lt };

inline ArmCond opposite_cond(ArmCond c) {
  constexpr static ArmCond OPPOSITE[] = {ArmCond::Any, ArmCond::Ne, ArmCond::Eq,
                                         ArmCond::Lt,  ArmCond::Le, ArmCond::Gt,
                                         ArmCond::Ge};
  return OPPOSITE[(int)c];
}

struct ArmShift {
  enum {
    // no shifting
    None,
    // arithmetic right
    Asr,
    // logic left
    Lsl,
    // logic right
    Lsr,
    // rotate right
    Ror,
    // rotate right one bit with extend
    Rrx
  } type;
  int shift;

  ArmShift() {
    shift = 0;
    type = None;
  }

  bool is_none() const { return type == None; }

  explicit operator std::string() const {
    const char* name;
    switch (type) {
      case ArmShift::Asr:
        name = "asr";
        break;
      case ArmShift::Lsl:
        name = "lsl";
        break;
      case ArmShift::Lsr:
        name = "lsr";
        break;
      case ArmShift::Ror:
        name = "ror";
        break;
      case ArmShift::Rrx:
        name = "rrx";
        break;
      default:
        UNREACHABLE();
    }
    return std::string(name) + " #" + std::to_string(shift);
  }
};

inline std::ostream& operator<<(std::ostream& os, const ArmShift& shift) {
  os << std::string(shift);
  return os;
}

inline std::ostream& operator<<(std::ostream& os, const ArmCond& cond) {
  if (cond == ArmCond::Eq) {
    os << "eq";
  } else if (cond == ArmCond::Ne) {
    os << "ne";
  } else if (cond == ArmCond::Any) {
    os << "";
  } else if (cond == ArmCond::Gt) {
    os << "gt";
  } else if (cond == ArmCond::Ge) {
    os << "ge";
  } else if (cond == ArmCond::Lt) {
    os << "lt";
  } else if (cond == ArmCond::Le) {
    os << "le";
  } else {
    UNREACHABLE();
  }
  return os;
}

struct MachineProgram {
  IntrusiveList<MachineFunc> func;
  std::vector<Decl*> glob_decl;
  friend std::ostream& operator<<(std::ostream& os, const MachineProgram& dt);
};

std::ostream& operator<<(std::ostream& os, const MachineProgram& dt);

struct MachineFunc {
  DEFINE_LIST(MachineFunc)
  IntrusiveList<MachineBB> bb;
  IrFunc* func;
  // number of virtual registers allocated
  u32 virtual_max = 0;
  // size of stack allocated for local alloca and spilled registers
  u32 stack_size = 0;
  // set of callee saved registers used
  std::set<ArmReg> used_callee_saved_regs;
  // whether lr is allocated
  bool use_lr = false;
  // offset += stack_size + saved_regs * 4;
  std::vector<MachineInst*> sp_arg_fixup;
};

struct MachineBB {
  DEFINE_LIST(MachineBB)
  BasicBlock* bb;
  IntrusiveList<MachineInst> insts;
  // predecessor and successor
  std::vector<MachineBB*> pred;
  std::array<MachineBB*, 2> succ;
  // branch is translated into multiple instructions
  // points to the first one
  MachineInst* control_transfer_inst = nullptr;
  // liveness analysis
  // maybe we should use bitset when performance is bad
  std::set<MachineOperand> liveuse;
  std::set<MachineOperand> def;
  std::set<MachineOperand> livein;
  std::set<MachineOperand> liveout;
};

struct MachineOperand {
  enum class State {
    PreColored,
    Allocated,
    Virtual,
    Immediate,
  } state;
  i32 value;

  inline static MachineOperand R(ArmReg r) {
    auto n = (int)r;
    assert(n >= int(ArmReg::r0) && n <= int(ArmReg::pc));
    return MachineOperand{State::PreColored, n};
  }

  inline static MachineOperand V(int n) {
    return MachineOperand{State::Virtual, n};
  }

  inline static MachineOperand I(int imm) {
    return MachineOperand{State::Immediate, imm};
  }

  // both are PreColored or Allocated, and has the same value
  bool is_equiv(const MachineOperand& other) const {
    return (state == State::PreColored || state == State::Allocated) &&
           (other.state == State::PreColored ||
            other.state == State::Allocated) &&
           value == other.value;
  }

  bool operator<(const MachineOperand& other) const {
    if (state != other.state) {
      return state < other.state;
    } else {
      return value < other.value;
    }
  }

  bool operator==(const MachineOperand& other) const {
    return state == other.state && value == other.value;
  }

  bool operator!=(const MachineOperand& other) const {
    return state != other.state || value != other.value;
  }

  bool is_virtual() const { return state == State::Virtual; }
  bool is_imm() const { return state == State::Immediate; }
  bool is_precolored() const { return state == State::PreColored; }
  bool is_reg() const {
    return state == State::PreColored || state == State::Allocated ||
           state == State::Virtual;
  }
  bool needs_color() const {
    return state == State::Virtual || state == State::PreColored;
  }

  explicit operator std::string() const {
    char prefix = '?';
    switch (this->state) {
      case State::PreColored:
      case State::Allocated:
        prefix = 'r';
        break;
      case State::Virtual:
        prefix = 'v';
        break;
      case State::Immediate:
        prefix = '#';
        break;
      default:
        UNREACHABLE();
    }
    return prefix + std::to_string(this->value);
  }

  friend std::ostream& operator<<(std::ostream& os, const MachineOperand& op) {
    os << std::string(op);
    return os;
  }
};

namespace std {
template <>
struct hash<MachineOperand> {
  std::size_t operator()(MachineOperand const& m) const noexcept {
    // state (2), value (14)
    return ((((size_t)m.state) << 14u) | (u32)m.value) & 0xFFFFu;
  }
};

template <>
struct hash<std::pair<MachineOperand, MachineOperand>> {
  std::size_t operator()(
      std::pair<MachineOperand, MachineOperand> const& m) const noexcept {
    // hash(second), hash(first)
    hash<MachineOperand> hash_func;
    return (hash_func(m.second) << 16u) | hash_func(m.first);
  }
};
}  // namespace std

struct MachineInst {
  DEFINE_LIST(MachineInst)
  MachineBB* bb;

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
    LongMul,
    FMA,
    Mv,
    Branch,
    Jump,
    Return,  // Control flow
    Load,
    Store,  // Memory
    Compare,
    Call,
    Global,
    Comment,  // for printing comments
  } tag;

  MachineInst(Tag tag, MachineBB* InsertAtEnd) : bb(InsertAtEnd), tag(tag) {
    if (InsertAtEnd) {
      InsertAtEnd->insts.InsertAtEnd(this);
    }
  }
  MachineInst(Tag tag, MachineInst* InsertBefore)
      : bb(InsertBefore->bb), tag(tag) {
    if (bb) {
      bb->insts.InsertBefore(this, InsertBefore);
    }
  }
  MachineInst(Tag tag) : tag(tag) {}
};

struct MIBinary : MachineInst {
  // Add, Sub, Rsb, Mul, Div, Mod, Lt, Le, Ge, Gt, Eq, Ne, And, Or
  DEFINE_CLASSOF(MachineInst, Tag::Add <= p->tag && p->tag <= Tag::Or);
  MachineOperand dst;
  MachineOperand lhs;
  MachineOperand rhs;
  ArmShift shift;

  MIBinary(Tag tag, MachineBB* InsertAtEnd) : MachineInst(tag, InsertAtEnd) {}

  bool isIdentity() {
    switch (tag) {
      case Tag::Add:
      case Tag::Sub:
        return dst.is_equiv(lhs) && rhs == MachineOperand::I(0) &&
               shift.type == ArmShift::None;
      default:
        return false;
    }
  }
};

struct MIQuaternary : MachineInst {
  // LongMul, FMA
  DEFINE_CLASSOF(MachineInst, Tag::LongMul <= p->tag && p->tag <= Tag::FMA);
  MachineOperand lhs;
  MachineOperand rhs;

  MIQuaternary(Tag tag, MachineBB* InsertAtEnd)
      : MachineInst(tag, InsertAtEnd) {}
};

struct MILongMul : MIQuaternary {
  DEFINE_CLASSOF(MachineInst, Tag::LongMul == p->tag);
  MachineOperand dst;

  explicit MILongMul(MachineBB* InsertAtEnd)
      : MIQuaternary(Tag::LongMul, InsertAtEnd) {}
};

struct MIFma : MIQuaternary {
  DEFINE_CLASSOF(MachineInst, Tag::FMA == p->tag);
  MachineOperand acc;
  MachineOperand dst;
  bool add;
  bool sign;
  ArmCond cond;

  explicit MIFma(bool add, bool sign, MachineBB* InsertAtEnd)
      : MIQuaternary(Tag::FMA, InsertAtEnd),
        add(add),
        sign(sign),
        cond(ArmCond::Any) {}
};

struct MIMove : MachineInst {
  DEFINE_CLASSOF(MachineInst, p->tag == Tag::Mv);
  ArmCond cond;
  MachineOperand dst;
  MachineOperand rhs;
  ArmShift shift;

  bool is_simple() {
    return cond == ArmCond::Any && shift.type == ArmShift::None;
  }

  MIMove(MachineBB* InsertAtEnd)
      : MachineInst(Tag::Mv, InsertAtEnd), cond(ArmCond::Any) {}
  MIMove(MachineBB* InsertAtBegin, int)
      : MachineInst(Tag::Mv), cond(ArmCond::Any) {
    if (InsertAtBegin) {
      bb = InsertAtBegin;
      InsertAtBegin->insts.InsertAtBegin(this);
    }
  }
  MIMove(MachineInst* InsertBefore)
      : MachineInst(Tag::Mv, InsertBefore), cond(ArmCond::Any) {}
};

struct MIMoveCompare {
  bool operator()(MIMove* const& lhs, const MIMove* const& rhs) const {
    if (lhs->cond != rhs->cond) return lhs->cond < rhs->cond;
    if (lhs->dst != rhs->dst) return lhs->dst < rhs->dst;
    if (lhs->rhs != rhs->rhs) return lhs->rhs < rhs->rhs;
    return false;
  }
};

struct MIBranch : MachineInst {
  DEFINE_CLASSOF(MachineInst, p->tag == Tag::Branch);
  ArmCond cond;
  MachineBB* target;
  MIBranch(MachineBB* InsertAtEnd) : MachineInst(Tag::Branch, InsertAtEnd) {}
};

struct MIJump : MachineInst {
  DEFINE_CLASSOF(MachineInst, p->tag == Tag::Jump);
  MachineBB* target;

  MIJump(MachineBB* target, MachineBB* InsertAtEnd)
      : MachineInst(Tag::Jump, InsertAtEnd), target(target) {}
};

struct MIReturn : MachineInst {
  DEFINE_CLASSOF(MachineInst, p->tag == Tag::Return);
  MIReturn(MachineBB* InsertAtEnd) : MachineInst(Tag::Return, InsertAtEnd) {}
};

struct MIAccess : MachineInst {
  DEFINE_CLASSOF(MachineInst, p->tag == Tag::Load || p->tag == Tag::Store);
  enum class Mode {
    Offset,
    Prefix,
    Postfix,
  } mode;
  MachineOperand addr;
  MachineOperand offset;
  i32 shift;
  ArmCond cond;
  MIAccess(MachineInst::Tag tag, MachineBB* InsertAtEnd)
      : MachineInst(tag, InsertAtEnd), cond(ArmCond::Any) {}
  MIAccess(MachineInst::Tag tag, MachineInst* InsertBefore)
      : MachineInst(tag, InsertBefore), cond(ArmCond::Any) {}
  MIAccess(MachineInst::Tag tag) : MachineInst(tag), cond(ArmCond::Any) {}
};

struct MILoad : MIAccess {
  DEFINE_CLASSOF(MachineInst, p->tag == Tag::Load);
  MachineOperand dst;

  MILoad(MachineBB* InsertAtEnd) : MIAccess(Tag::Load, InsertAtEnd) {}
  MILoad(MachineInst* InsertBefore) : MIAccess(Tag::Load, InsertBefore) {}
  MILoad(MachineBB* InsertAtBegin, int) : MIAccess(Tag::Load) {
    bb = InsertAtBegin;
    InsertAtBegin->insts.InsertAtBegin(this);
  }
};

struct MIStore : MIAccess {
  DEFINE_CLASSOF(MachineInst, p->tag == Tag::Store);
  MachineOperand data;

  explicit MIStore(MachineBB* InsertAtEnd)
      : MIAccess(Tag::Store, InsertAtEnd) {}
  MIStore() : MIAccess(Tag::Store) {}
};

struct MICompare : MachineInst {
  DEFINE_CLASSOF(MachineInst, p->tag == Tag::Compare);
  MachineOperand lhs;
  MachineOperand rhs;

  explicit MICompare(MachineBB* InsertAtEnd)
      : MachineInst(Tag::Compare, InsertAtEnd) {}
};

struct MICall : MachineInst {
  DEFINE_CLASSOF(MachineInst, p->tag == Tag::Call);
  Func* func;

  explicit MICall(MachineBB* InsertAtEnd)
      : MachineInst(Tag::Call, InsertAtEnd) {}
};

struct MIGlobal : MachineInst {
  DEFINE_CLASSOF(MachineInst, p->tag == Tag::Global);
  MachineOperand dst;
  Decl* sym;

  MIGlobal(Decl* sym, MachineBB* InsertAtBegin)
      : MachineInst(Tag::Global), sym(sym) {
    InsertAtBegin->insts.InsertAtBegin(this);
  }
};

struct MIComment : MachineInst {
  DEFINE_CLASSOF(MachineInst, p->tag == Tag::Comment);
  std::string content;

  MIComment(std::string&& content, MachineBB* InsertAtEnd)
      : MachineInst(Tag::Comment, InsertAtEnd), content(content) {}
  MIComment(std::string&& content, MachineInst* InsertBefore)
      : MachineInst(Tag::Comment, InsertBefore), content(content) {}
};
