#include "conv/typeck.hpp"

#include <cstdio>
#include <string>
#include <unordered_map>

#include "casting.hpp"
#include "common.hpp"
#include "structure/op.hpp"

Func* Env::LookupFunc(std::string name) {
  auto res = glob.find(name);
  if (res != glob.end()) {
    if (Func* f = res->second.AsFunc()) {
      return f;
    }
  }
  ERROR("no such function: {}", name);
}

Decl* Env::LookupDecl(std::string name) {
  for (auto it = local_stk.rbegin(); it < local_stk.rend(); ++it) {
    auto res = it->find(name);
    if (res != it->end()) {
      return res->second;
    }
  }
  auto res = glob.find(name);
  if (res != glob.end()) {
    if (Decl* d = res->second.AsDecl()) {
      return d;
    }
  }
  ERROR("no such variable: {}", name);
}

void Env::CheckFunc(Func* f) {
  cur_func = f;
  if (!glob.insert({f->name, Symbol::MakeFunc(f)}).second) {
    ERROR("duplicate function: {}", f->name);
  }
  local_stk.emplace_back();  // 参数作用域就是第一层局部变量的作用域
  for (Decl& d : f->params) {
    CheckDecl(d);
    if (!local_stk[0].insert({d.name, &d}).second) {
      ERROR("duplicate parameter: {}", d.name);
    }
  }
  for (Stmt* s : f->body.stmts) {
    CheckStmt(s);
  }
  local_stk.pop_back();
}

// 数组和标量的初始化都会以 FlattenInitList 的形式存储
// 即使没有初始化，全局变量也会以 0 初始化，而局部变量的 FlattenInitList
// 这时是空的
void Env::CheckDecl(Decl& d) {
  // 每个维度保存的 result 是包括它在内右边所有维度的乘积
  for (auto it = d.dims.rbegin(); it < d.dims.rend(); ++it) {
    Expr* e = *it;
    // in function parameter the first array dimension can be empty
    if (e != nullptr) {
      Eval(e);
      if (e->result < 0) {
        ERROR("array dimension cannot less than 0");
      }
      // ref:
      // https://stackoverflow.com/questions/11868087/avoiding-powers-of-2-for-cache-friendliness
      if (e->result >= 256 && (e->result & (e->result - 1)) == 0) {
        spdlog::debug("extend dimension from {} to {}", e->result,
                      e->result + 10);
        e->result += 10;
      }
      if (it != d.dims.rbegin()) {
        e->result *= it[-1]->result;
      }
    }
  }
  if (d.has_init) {
    if (d.dims.empty() && (d.init.val1 != nullptr)) {
      // like `int x = 1`
      CheckExpr(d.init.val1);
      if (d.is_const || d.is_glob) {
        Eval(d.init.val1);
      }
      d.FlattenInitList.push_back(d.init.val1);
    } else if (d.init.val1 == nullptr) {
      // like `int x[2] = {1， 2}`
      FlattenInitList(d.init.val2, d.dims.data(), d.dims.data() + d.dims.size(),
                      d.is_const | d.is_glob, d.FlattenInitList);
    } else {
      ERROR("incompatible initialization");
    }
  } else if (d.is_const) {
    ERROR("const declaration must have initialization");
  } else if (d.is_glob) {
    d.FlattenInitList.resize(d.dims.empty() ? 1 : d.dims[0]->result,
                             new IntConst{{Expr::IntConst, 0}, 0});
  }
}

void Env::CheckStmt(Stmt* s) {
  if (auto* x = dyn_cast<Assign>(s)) {
    Decl* d = LookupDecl(x->ident);
    x->lhs_sym = d;
    if (d->is_const) {
      ERROR("cannot assign to const variable");
    }
    for (Expr* e : x->dims) {
      if (!IsInt(CheckExpr(e))) {
        ERROR("integer operator expect integer operand");
      }
    }
    if (!(IsInt(CheckExpr(x->rhs)) && d->dims.size() == x->dims.size())) {
      ERR("can only assign int to int");
    }
  } else if (auto* x = dyn_cast<ExprStmt>(s)) {
    // 不要检查分号
    if (x->val) CheckExpr(x->val);
  } else if (auto* x = dyn_cast<DeclStmt>(s)) {
    auto& top = local_stk.back();
    for (Decl& d : x->decls) {
      CheckDecl(d);
      if (!top.insert({d.name, &d}).second) {
        ERR("duplicate local decl", d.name);
      }
    }
  } else if (auto* x = dyn_cast<Block>(s)) {
    local_stk.emplace_back();
    for (Stmt* s : x->stmts) {
      CheckStmt(s);
    }
    local_stk.pop_back();
  } else if (auto* x = dyn_cast<If>(s)) {
    if (!IsInt(CheckExpr(x->cond))) {
      ERR("cond isn't int type");
    }
    CheckStmt(x->on_true);
    if (x->on_false != nullptr) {
      CheckStmt(x->on_false);
    }
  } else if (auto* x = dyn_cast<While>(s)) {
    if (!IsInt(CheckExpr(x->cond))) {
      ERR("cond isn't int type");
    }
    ++loop_cnt;
    CheckStmt(x->body);
    --loop_cnt;
  } else if (isa<Break>(s)) {
    if (loop_cnt == 0U) {
      ERROR("break statement outside loop");
    }
  } else if (isa<Continue>(s)) {
    if (loop_cnt == 0U) {
      ERROR("continue statement outside loop");
    }
  } else if (auto* x = dyn_cast<Return>(s)) {
    auto t =
        x->val != nullptr ? CheckExpr(x->val) : std::pair<Expr**, Expr**>{};
    if (!((cur_func->is_int && IsInt(t)) || (!cur_func->is_int && !t.first))) {
      ERROR("return type mismatch");
    }
  } else {
    UNREACHABLE();
  }
}

// src 应该都是没有 Eval 过的；这里只处理必须是列表形式的 src，不处理单独 expr
// 形式的 [dims, dims_end) 组成一个非空 slice；dims 应该符合 Decl::dims
// 的描述，且已经求值完毕 FlattenInitList 可以用于常量和非常量的初始化，由
// need_eval 控制
void Env::FlattenInitList(std::vector<InitList>& src, Expr** dims,
                          Expr** dims_end, bool need_eval,
                          std::vector<Expr*>& dst) {
  u32 elem_size = dims + 1 < dims_end ? dims[1]->result : 1,
      expect = dims[0]->result;
  u32 cnt = 0, old_len = dst.size();
  for (InitList& l : src) {
    if (l.val1) {
      CheckExpr(l.val1);
      if (need_eval) {
        Eval(l.val1);
      }
      dst.push_back(l.val1);
      if (++cnt == elem_size) {
        cnt = 0;
      }
    } else {
      // 遇到了一个新的列表，它必须恰好填充一个元素
      // 给前一个未填满的元素补 0
      if (cnt != 0) {
        dst.resize(dst.size() + elem_size - cnt,
                   new IntConst{{Expr::IntConst, 0}, 0});
        cnt = 0;
      }
      if (dims < dims_end) {
        FlattenInitList(l.val2, dims + 1, dims_end, need_eval, dst);
      } else {
        ERR("initializer list has too many dimensions");
      }
    }
  }
  u32 actual = dst.size() - old_len;
  if (actual <= expect) {
    dst.resize(dst.size() + expect - actual,
               new IntConst{{Expr::IntConst, 0}, 0});
  } else {
    ERR("too many initializing values", expect, actual);
  }
}

// 配合 CheckExpr 使用
bool Env::IsInt(std::pair<Expr**, Expr**> t) {
  return (t.first != nullptr) && t.first == t.second;
}

// 返回 Option<&[Expr *]>：
// 类型是 int 时返回空 slice，类型是 void 时返回 None，类型是
// int[]...时返回对应维度的 slice
std::pair<Expr**, Expr**> Env::CheckExpr(Expr* e) {
  const std::pair<Expr**, Expr**> none{};
  const std::pair<Expr**, Expr**> empty{reinterpret_cast<Expr**>(8),
                                        reinterpret_cast<Expr**>(8)};
  if (auto* x = dyn_cast<Binary>(e)) {
    auto l = CheckExpr(x->lhs);
    auto r = CheckExpr(x->rhs);
    if (!IsInt(l) && !IsInt(r)) ERROR("binary operator expect integer operand");

    return empty;
  }
  if (auto* x = dyn_cast<Call>(e)) {
    Func* f = LookupFunc(x->func);
    x->f = f;
    if (f->params.size() != x->args.size())
      ERROR("function call argument count mismatch");

    for (u32 i = 0; i < x->args.size(); ++i) {
      auto a = CheckExpr(x->args[i]);
      std::vector<Expr*>& p = f->params[i].dims;
      // 跳过第一个维度的检查，因为函数参数的第一个维度为空
      bool ok = (a.first != nullptr) &&
                static_cast<size_t>(a.second - a.first) == p.size();
      for (u32 j = 1, end = p.size(); ok && j < end; ++j) {
        if (a.first[j]->result != p[j]->result) {
          ok = false;
        }
      }
      if (!ok) {
        ERR("function call arg mismatch", f->name, i + 1, f->params[i].name);
      }
    }
    return f->is_int ? empty : none;
  }
  if (auto* x = dyn_cast<Index>(e)) {
    // 这里允许不完全解引用数组
    Decl* d = LookupDecl(x->name);
    x->lhs_sym = d;
    if (x->dims.size() > d->dims.size()) {
      ERR("index operator expect array operand");
    }
    for (Expr* e : x->dims) {
      if (!IsInt(CheckExpr(e))) {
        ERROR("integer operator expect integer operand");
      }
    }
    // 这里逻辑上总是返回：后面的，但是 stl 的实现中空 vector 的指针可能是
    // nullptr，所以加个特判
    return d->dims.empty() ? empty
                           : std::pair{d->dims.data() + x->dims.size(),
                                       d->dims.data() + d->dims.size()};
  }
  if (auto* x = dyn_cast<IntConst>(e)) {
    e->result = x->val;
    return empty;
  }
  UNREACHABLE();
}

void Env::Eval(Expr* e) {
  if (auto* x = dyn_cast<Binary>(e)) {
    Eval(x->lhs), Eval(x->rhs);
    x->result =
        op::Eval(static_cast<op::Op>(x->tag), x->lhs->result, x->rhs->result);
  } else if (isa<Call>(e)) {
    ERROR("const expression disallow function call");
  } else if (auto* x = dyn_cast<Index>(e)) {
    Decl* d = LookupDecl(x->name);
    if (!d->is_const) {
      ERROR("non const variable {} used in const expression", x->name);
    }
    if (d->dims.size() != x->dims.size()) {
      ERROR("array dimension mismatch");
    }
    u32 off = 0;
    // 因为没有保存每个维度的长度，所以没法在每一个下标处都检查了，不过这也没什么关系
    for (u32 i = 0; i < x->dims.size(); ++i) {
      Expr* idx = x->dims[i];
      Eval(idx);
      off +=
          (i + 1 == x->dims.size() ? 1 : d->dims[i + 1]->result) * idx->result;
    }
    if (off >= d->FlattenInitList.size()) {
      ERROR("array index out of range");
    }
    x->result = d->FlattenInitList[off]->result;
  } else if (auto* x = dyn_cast<IntConst>(e)) {
    x->result = x->val;
  } else {
    UNREACHABLE();
  }
}

void TypeCheck(Program& p) {
  Env env;
  for (Func& f : Func::builtin_function) {
    env.CheckFunc(&f);
  }
  for (auto& g : p.glob) {
    if (Func* f = std::get_if<0>(&g)) {
      env.CheckFunc(f);
    } else {
      Decl* d = std::get_if<1>(&g);
      env.CheckDecl(*d);
      if (!env.glob.insert({d->name, Symbol::MakeDecl(d)}).second) {
        ERROR("duplicate global declaration: {}", d->name);
      }
    }
  }
}
