#include "machine_code.hpp"

#include "casting.hpp"

std::ostream &operator<<(std::ostream &os, const MachineProgram &p) {
  using std::endl;
  static const std::string BB_PREFIX = "_BB";

  // code section
  os << ".section .text" << endl;
  for (auto f = p.func.head; f; f = f->next) {
    // generate symbol for function
    os << ".global " << f->func->func->name << endl;
    os << f->func->func->name << ":" << endl;
    os << "\t" << ".type" << "\t" << f->func->func->name << ", %function" << endl;

    IndexMapper<MachineBB> bb_index;
    // generate code for each BB
    for (auto bb = f->bb.head; bb; bb = bb->next) {
      os << BB_PREFIX << bb_index.get(bb) << ":" << endl;
      for (auto inst = bb->insts.head; inst; inst = inst->next) {
        os << "\t";
        if (auto x = dyn_cast<MIJump>(inst)) {
          os << "b" << "\t" << BB_PREFIX << bb_index.get(x->target) << endl;
        } else if (auto x = dyn_cast<MIBranch>(inst)) {
          os << "b" << "\t" << BB_PREFIX << bb_index.get(x->target) << endl;
        } else if (auto x = dyn_cast<MILoad>(inst)) {
          if (x->offset.state == MachineOperand::Immediate) {
            i32 offset = x->offset.value << x->shift;
            os << "ldr" << "\t" << x->dst << ", [" << x->addr << ", #" << offset << "]" << endl;
          } else {
            os << "ldr" << "\t" << x->dst << ", [" << x->addr << ", " << x->offset << ", LSL #" << x->shift << "]" << endl;
          }
        } else if (auto x = dyn_cast<MIStore>(inst)) {
          os << "str" << "\t" << x->data << ", [" << x->addr << "]" << endl;
        } else if (auto x = dyn_cast<MIGlobal>(inst)) {
          os << "ldr" << "\t" << x->dst << ", =" << x->sym->name << endl;
        } else if (auto x = dyn_cast<MIBinary>(inst)) {
          const char *op = "unknown";
          if (x->tag == MachineInst::Mul) {
            op = "mul";
          } else if (x->tag == MachineInst::Add) {
            op = "add";
          } else if (x->tag == MachineInst::Sub) {
            op = "sub";
          } else if (x->tag == MachineInst::Mod) {
            op = "mod";
          } else {
            UNREACHABLE();
          }
          os << op << "\t" << x->dst << ", " << x->lhs << ", " << x->rhs << endl;
        } else if (auto x = dyn_cast<MIUnary>(inst)) {
          if (x->tag == MachineInst::Mv) {
            os << "mov" << "\t" << x->dst << ", " << x->rhs << endl;
          } else {
            UNREACHABLE();
          }
        } else if (auto x = dyn_cast<MICompare>(inst)) {
          os << "cmp" << "\t" << x->lhs << ", " << x->rhs << endl;
        } else if (auto x = dyn_cast<MIMove>(inst)) {
          os << "mov" << "\t" << x->cond << " " << x->dst << ", " << x->rhs << endl;
        } else if (auto x = dyn_cast<MIReturn>(inst)) {
          os << "bx" << "\t" << "lr" << endl;
        }
      }
    }
  }
  // data section
  os << ".section .data" << endl;
  for (auto &decl : p.glob_decl) {
    os << ".global " << decl->name << endl;
    os << decl->name << ":" << endl;
    for (auto expr : decl->flatten_init) {
      os << "\t"
         << ".long " << expr->result << endl;
    }
  }
  return os;
}