#include "passes/ir/memdep.hpp"

#include <unordered_map>

#include "passes/ir/cfg.hpp"
#include "structure/ast.hpp"

// 如果一个是另一个的 postfix，则可能 alias；nullptr 相当于通配符
static bool dim_alias(const std::vector<Expr*>& dim1,
                      const std::vector<Expr*>& dim2) {
  auto pred = [](Expr* l, Expr* r) {
    return !l || !r || l->result == r->result;
  };
  return dim1.size() < dim2.size() ? std::equal(dim1.begin(), dim1.end(),
                                                dim2.end() - dim1.size(), pred)
                                   : std::equal(dim2.begin(), dim2.end(),
                                                dim1.end() - dim2.size(), pred);
}

// 目前只考虑用数组的类型/维度来排除alias，不考虑用下标来排除
// 分三种情况：!dims.empty() && dims[0] == nullptr => 参数数组; 否则 is_glob ==
// true => 全局变量; 否则是局部数组
// 这个关系是对称的，但不是传递的，例如参数中的 int [] 和 int [][5]，int
// [][10] 都 alias，但 int [][5] 和 int [][10] 不 alias
bool alias(Decl* arr1, Decl* arr2) {
  if (arr1->IsParamArray()) {  // 参数
    if (arr2->IsParamArray())
      // NOTE_OPT: this assumes that any two arrays in parameters do not alias
      return arr1->name == arr2->name;
    else if (arr2->is_glob)
      return dim_alias(arr1->dims, arr2->dims);
    else
      return false;
  } else if (arr1->is_glob) {  // 全局变量
    if (arr2->IsParamArray())
      return dim_alias(arr1->dims, arr2->dims);
    else if (arr2->is_glob)
      return arr1 == arr2;
    else
      return false;
  } else {  // 局部数组
    if (arr2->IsParamArray())
      return false;
    else if (arr2->is_glob)
      return false;
    else
      return arr1 == arr2;
  }
}

// 如果 load 的数组不是本函数内定义的，一个函数调用就可能修改其内容，这包括
// ParamRef 和 GlobalRef 如果 load 的数组是本函数内定义的，即是
// AllocaInst，则只有当其地址被不完全 load
// 作为参数传递给一个函数时，这个函数才可能修改它
bool is_arr_call_alias(Decl* arr, CallInst* y) {
  return arr->IsParamArray() || arr->is_glob ||
         std::any_of(y->args.begin(), y->args.end(), [arr](Use& u) {
           if (auto a = dyn_cast<GetElementPtrInst>(u.value);
               a && alias(arr, a->lhs_sym))
             return true;
           if (auto a = dyn_cast<AllocaInst>(u.value); a && arr == a->sym)
             return true;
           return false;
         });
}

struct LoadInfo {
  u32 id;
  std::vector<LoadInst*> loads;
  std::unordered_set<Inst*> stores;
};

void clear_memdep(IrFunc* f) {
  // 如果在同一趟循环中把操作数.set(nullptr)，同时 delete，会出现先被 delete
  // 后维护它的 uses 链表的情况，所以分两趟循环 这里也不能用.value =
  // nullptr，因为不能保证用到的指令最终都被删掉了，例如 MemOpInst 的 mem_token
  // 可以是 LoadInst
  for (BasicBlock* bb = f->bb.head; bb; bb = bb->next) {
    for (Inst* i = bb->mem_phis.head; i; i = i->next) {
      auto i1 = static_cast<MemPhiInst*>(i);
      for (Use& u : i1->incoming_values) u.Set(nullptr);
    }
    for (Inst* i = bb->instructions.head; i; i = i->next) {
      if (auto x = dyn_cast<MemOpInst>(i))
        x->mem_token.Set(nullptr);
      else if (auto x = dyn_cast<LoadInst>(i))
        x->mem_token.Set(nullptr);
    }
  }
  for (BasicBlock* bb = f->bb.head; bb; bb = bb->next) {
    for (Inst* i = bb->instructions.head; i; i = i->next) {
      for (Use* u = i->uses.head; u; u = u->next) {
        assert(!isa<MemOpInst>(u->user) && !isa<MemPhiInst>(u->user));
      }
    }
  }
  for (BasicBlock* bb = f->bb.head; bb; bb = bb->next) {
    for (Inst* i = bb->mem_phis.head; i;) {
      Inst* next = i->next;
      delete static_cast<MemPhiInst*>(i);
      i = next;
    }
    bb->mem_phis.head = bb->mem_phis.tail = nullptr;
    for (Inst* i = bb->instructions.head; i;) {
      Inst* next = i->next;
      if (auto x = dyn_cast<MemOpInst>(i)) {
        bb->instructions.Remove(x);
        delete x;
      }
      i = next;
    }
  }
}

// 构造 load 对 store，store 对 load 的依赖关系，分成两趟分别计算
void compute_memdep(IrFunc* f) {
  compute_dom_info(f);
  // 把所有数组地址相同的 load 一起考虑，因为相关的 store
  // 集合计算出来必定是一样的
  std::unordered_map<Decl*, LoadInfo> loads;
  for (BasicBlock* bb = f->bb.head; bb; bb = bb->next) {
    for (Inst* i = bb->instructions.head; i; i = i->next) {
      if (auto x = dyn_cast<LoadInst>(i)) {
        Decl* arr = x->lhs_sym;
        auto [it, inserted] = loads.insert({arr, {(u32)loads.size()}});
        LoadInfo& info = it->second;
        info.loads.push_back(x);
        if (!inserted) continue;  // stores 已经计算过了
        for (BasicBlock* bb1 = f->bb.head; bb1; bb1 = bb1->next) {
          for (Inst* i1 = bb1->instructions.head; i1; i1 = i1->next) {
            bool is_alias = false;
            if (auto x = dyn_cast<StoreInst>(i1); x && alias(arr, x->lhs_sym))
              is_alias = true;
            // todo:
            // 这里可以更仔细地考虑到底是否修改了参数，现在是粗略的判断，如果没有
            // side effect一定没有修改参数/全局变量
            else if (auto x = dyn_cast<CallInst>(i1);
                     x && x->func->has_side_effect && is_arr_call_alias(arr, x))
              is_alias = true;
            if (is_alias) info.stores.insert(i1);
          }
        }
      }
    }
  }
  auto df = compute_df(f);
  // 第一趟，构造 load 对 store 的依赖关系
  {
    std::vector<BasicBlock*> worklist;
    for (auto& [arr, info] : loads) {
      f->ClearAllVis();
      for (Inst* i : info.stores) worklist.push_back(i->bb);
      while (!worklist.empty()) {
        BasicBlock* x = worklist.back();
        worklist.pop_back();
        for (BasicBlock* y : df[x]) {
          if (!y->vis) {
            y->vis = true;
            new MemPhiInst(arr, y);
            worklist.push_back(y);
          }
        }
      }
    }
    std::vector<std::pair<BasicBlock*, std::vector<Value*>>> worklist2{
        {f->bb.head, std::vector<Value*>(loads.size(), new UndefValue)}};
    f->ClearAllVis();
    while (!worklist2.empty()) {
      BasicBlock* bb = worklist2.back().first;
      std::vector<Value*> values = std::move(worklist2.back().second);
      worklist2.pop_back();
      if (!bb->vis) {
        bb->vis = true;
        for (Inst* i = bb->mem_phis.head; i; i = i->next) {
          auto i1 = static_cast<MemPhiInst*>(i);
          values[loads.find(static_cast<Decl*>(i1->load_or_arr))->second.id] =
              i;
        }
        for (Inst* i = bb->instructions.head; i; i = i->next) {
          if (auto x = dyn_cast<LoadInst>(i)) {
            x->mem_token.Set(values[loads.find(x->lhs_sym)->second.id]);
          } else if (isa<StoreInst>(i) || isa<CallInst>(i)) {
            for (auto& load_info : loads) {
              if (load_info.second.stores.find(i) !=
                  load_info.second.stores.end()) {
                values[load_info.second.id] = i;
              }
            }
          }
        }
        for (BasicBlock* x : bb->Succ()) {
          if (x) {
            worklist2.emplace_back(x, values);
            for (Inst* i = x->mem_phis.head; i; i = i->next) {
              auto i1 = static_cast<MemPhiInst*>(i);
              u32 id =
                  loads.find(static_cast<Decl*>(i1->load_or_arr))->second.id;
              u32 idx = std::find(x->pred.begin(), x->pred.end(), bb) -
                        x->pred.begin();
              i1->incoming_values[idx].Set(values[id]);
            }
          }
        }
      }
    }
  }
  // 第二趟，构造 store 对 load 的依赖关系，虽然 store 也依赖
  // store，但是后面不会调整 store 的位置，所以没有必要考虑这个依赖关系
  // 与第一趟不同，这里不能把一个地址的 load 放在一起考虑，比如连续两个
  // load，如果一起考虑的话就会认为前一个 load 不被任何 store 依赖
  std::unordered_map<LoadInst*, u32> loads2;
  for (auto& arr_info : loads) {
    for (LoadInst* load : arr_info.second.loads) {
      loads2.insert({load, (u32)loads2.size()});
      for (Inst* store : arr_info.second.stores) {
        new MemOpInst(load, store);
      }
    }
  }
  {
    std::vector<BasicBlock*> worklist;
    for (auto& load_id : loads2) {
      f->ClearAllVis();
      worklist.push_back(load_id.first->bb);
      while (!worklist.empty()) {
        BasicBlock* x = worklist.back();
        worklist.pop_back();
        for (BasicBlock* y : df[x]) {
          if (!y->vis) {
            y->vis = true;
            new MemPhiInst(load_id.first, y);
            worklist.push_back(y);
          }
        }
      }
    }
    std::vector<std::pair<BasicBlock*, std::vector<Value*>>> worklist2{
        {f->bb.head, std::vector<Value*>(loads2.size(), new UndefValue)}};
    f->ClearAllVis();
    while (!worklist2.empty()) {
      BasicBlock* bb = worklist2.back().first;
      std::vector<Value*> values = std::move(worklist2.back().second);
      worklist2.pop_back();
      if (!bb->vis) {
        bb->vis = true;
        for (Inst* i = bb->mem_phis.head; i; i = i->next) {
          auto i1 = static_cast<MemPhiInst*>(i);
          // 不考虑第一趟引入的 MemPhiInst
          if (auto it = loads2.find(static_cast<LoadInst*>(i1->load_or_arr));
              it != loads2.end()) {
            values[it->second] = i;
          }
        }
        for (Inst* i = bb->instructions.head; i; i = i->next) {
          if (auto x = dyn_cast<LoadInst>(i); x) {
            values[loads2.find(x)->second] = x;
          } else if (auto x = dyn_cast<MemOpInst>(i)) {
            x->mem_token.Set(values[loads2.find(x->load)->second]);
          }
        }
        for (BasicBlock* x : bb->Succ()) {
          if (x) {
            worklist2.emplace_back(x, values);
            for (Inst* i = x->mem_phis.head; i; i = i->next) {
              auto i1 = static_cast<MemPhiInst*>(i);
              if (auto it =
                      loads2.find(static_cast<LoadInst*>(i1->load_or_arr));
                  it != loads2.end()) {
                u32 idx = std::find(x->pred.begin(), x->pred.end(), bb) -
                          x->pred.begin();
                i1->incoming_values[idx].Set(values[it->second]);
              }
            }
          }
        }
      }
    }
  }
  // 删除无用的 MemPhi，避免不必要的依赖
  while (true) {
    bool changed = false;
    for (BasicBlock* bb = f->bb.head; bb; bb = bb->next) {
      for (Inst* i = bb->mem_phis.head; i;) {
        Inst* next = i->next;
        if (i->uses.head == nullptr) {
          bb->mem_phis.Remove(i);
          delete static_cast<MemPhiInst*>(i);
          changed = true;
        }
        i = next;
      }
    }
    if (!changed) break;
  }
}
