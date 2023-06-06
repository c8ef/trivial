#include "conv/parser.hpp"

#include <cassert>
#include <stack>

const int kOpPrecTable[] = {MIMIC_OPERATORS(MIMIC_EXPAND_THIRD)};

inline int GetOpPrec(Operator op) { return kOpPrecTable[static_cast<int>(op)]; }

inline Expr::Tag GetBinaryOp(Operator op) {
  switch (op) {
    case Operator::Add:
      return Expr::Add;
    case Operator::Sub:
      return Expr::Sub;
    case Operator::Mul:
      return Expr::Mul;
    case Operator::Div:
      return Expr::Div;
    case Operator::Mod:
      return Expr::Mod;
    case Operator::Equal:
      return Expr::Eq;
    case Operator::NotEqual:
      return Expr::Ne;
    case Operator::Less:
      return Expr::Lt;
    case Operator::LessEqual:
      return Expr::Le;
    case Operator::Great:
      return Expr::Gt;
    case Operator::GreatEqual:
      return Expr::Ge;
    case Operator::LogicAnd:
      return Expr::And;
    case Operator::LogicOr:
      return Expr::Or;
    default:
      assert(false);
      return Expr::Add;
  }
}

Program Parser::ParseProgram() {
  Program program;

  while (cur_token_ != Token::End) {
    bool is_const = false;
    Keyword type = Keyword::Int;

    if (IsTokenKeyword(Keyword::Const)) {
      is_const = true;
      NextToken();
    }

    if (IsTokenKeyword(Keyword::Int)) {
      NextToken();
    } else if (IsTokenKeyword(Keyword::Void)) {
      type = Keyword::Void;
      NextToken();
    } else {
      spdlog::error("Parser error: invalid type");
      return {};
    }

    if (!ExpectId()) {
      return {};
    }

    std::string id = lexer_.IdVal();
    NextToken();

    if (IsTokenChar('(')) {
      Func func = ParseFunction(type, id);
      program.glob.emplace_back(func);
    } else {
      std::vector<Decl> decls = ParseDecl(is_const, id);
      for (Decl& decl : decls) {
        decl.is_glob = true;
        program.glob.emplace_back(decl);
      }
    }
  }

  return program;
}

Func Parser::ParseFunction(Keyword ret_type, const std::string& id) {
  NextToken();
  std::vector<Decl> params;

  if (!IsTokenChar(')')) {
    for (;;) {
      Decl param = ParseParam();
      params.push_back(param);

      if (!IsTokenChar(',')) {
        break;
      }
      NextToken();
    }
  }
  if (!ExpectChar(')')) {
    assert(false);
  }

  Stmt* block = ParseBlock();
  if (block == nullptr) {
    assert(false);
  }

  if (ret_type == Keyword::Int) {
    return Func{true, id, params, *reinterpret_cast<Block*>(block)};
  }
  return Func{false, id, params, *reinterpret_cast<Block*>(block)};
}

Decl Parser::ParseParam() {
  if (!IsTokenKeyword(Keyword::Int)) {
    assert(false);
  }
  NextToken();

  if (!ExpectId()) {
    assert(false);
  }

  std::string id = lexer_.IdVal();
  NextToken();

  std::vector<Expr*> dims;
  if (IsTokenChar('[')) {
    dims = ParseArrayDims0();
    return Decl{false, false, false, id, dims, {nullptr}};
  }
  return Decl{false, false, false, id, {}, {nullptr}};
}

std::vector<Decl> Parser::ParseDecl(bool is_const,
                                    const std::string& first_id) {
  std::vector<Decl> decls;
  // now cur token pass the first id
  auto first_dims = ParseArrayDims();
  if (IsTokenOperator(Operator::Assign)) {
    NextToken();
    auto init_list = ParseInitList();
    decls.push_back(Decl{false, false, true, first_id, first_dims, init_list});
  } else {
    decls.push_back(Decl{false, false, false, first_id, first_dims, {}});
  }

  while (!IsTokenChar(';')) {
    NextToken();  // eat ','

    if (!ExpectId()) {
      return {};
    }

    std::string id = lexer_.IdVal();
    NextToken();

    auto latter_dims = ParseArrayDims();
    if (IsTokenOperator(Operator::Assign)) {
      NextToken();
      auto latter_init_list = ParseInitList();
      decls.push_back(
          Decl{false, false, true, id, latter_dims, latter_init_list});
    } else {
      decls.push_back(Decl{false, false, false, id, latter_dims, {}});
    }
  }

  if (!ExpectChar(';')) {
    assert(false);
  }
  if (is_const) {
    for (Decl& decl : decls) {
      decl.is_const = true;
    }
  }
  return decls;
}

InitList Parser::ParseInitList() {
  if (!IsTokenChar('{')) {
    return InitList{ParseExpr(), {}};
  }
  NextToken();

  std::vector<InitList> lists;
  while (!IsTokenChar('}')) {
    InitList list = ParseInitList();
    lists.push_back(list);

    if (!IsTokenChar(',')) {
      break;
    }
    NextToken();
  }

  if (!ExpectChar('}')) {
    assert(false);
  }
  return InitList{nullptr, lists};
}

Stmt* Parser::ParseStmt() {
  if (IsTokenChar('{')) {
    return ParseBlock();
  }

  if (IsTokenChar(';')) {
    return ParseEmptyStmt();
  }

  if (cur_token_ == Token::Keyword) {
    switch (lexer_.KeyVal()) {
      case Keyword::If:
        return ParseIfElse();
      case Keyword::While:
        return ParseWhile();
      case Keyword::Break:
        NextToken();
        if (!ExpectChar(';')) {
          return nullptr;
        }
        return new Break{Stmt::Break};
      case Keyword::Continue:
        NextToken();
        if (!ExpectChar(';')) {
          return nullptr;
        }
        return new Continue{Stmt::Continue};
      case Keyword::Return: {
        NextToken();
        Expr* expr = nullptr;
        if (!IsTokenChar(';')) {
          expr = ParseExpr();
          if (expr == nullptr) {
            return nullptr;
          }
        }
        if (!ExpectChar(';')) {
          return nullptr;
        }
        if (expr != nullptr) {
          return new Return{{Stmt::Return}, expr};
        }
        return new Return{{Stmt::Return}, nullptr};
      }
      case Keyword::Const: {
        NextToken();
        if (!IsTokenKeyword(Keyword::Int)) {
          return nullptr;
        }
        NextToken();
        if (!ExpectId()) {
          return nullptr;
        }
        std::string id = lexer_.IdVal();
        NextToken();
        std::vector<Decl> decls = ParseDecl(true, id);
        return new DeclStmt{{Stmt::DeclStmt}, decls};
      }
      case Keyword::Int: {
        NextToken();

        if (!ExpectId()) {
          return nullptr;
        }
        std::string id = lexer_.IdVal();
        NextToken();
        std::vector<Decl> decls = ParseDecl(false, id);
        return new DeclStmt{{Stmt::DeclStmt}, decls};
      }
      default:
        spdlog::error("Parser error: invalid keyword");
    }
  }

  return ParseBare();
}

Stmt* Parser::ParseEmptyStmt() {
  NextToken();
  return new ExprStmt{{Stmt::ExprStmt}, nullptr};
}

Stmt* Parser::ParseBare() {
  auto* expr = ParseExpr();
  if (expr->tag == Expr::Index && IsTokenOperator(Operator::Assign)) {
    NextToken();
    auto* rhs = ParseExpr();
    if (rhs == nullptr) return nullptr;
    if (!ExpectChar(';')) return nullptr;

    return new Assign{{Stmt::Assign},
                      (reinterpret_cast<Index*>(expr))->name,
                      (reinterpret_cast<Index*>(expr))->dims,
                      rhs};
  }
  if (expr == nullptr) return nullptr;
  if (!ExpectChar(';')) return nullptr;

  return new ExprStmt{{Stmt::ExprStmt}, expr};
}

Stmt* Parser::ParseBlock() {
  if (!ExpectChar('{')) {
    return nullptr;
  }

  std::vector<Stmt*> stmts;
  while (!IsTokenChar('}')) {
    auto* stmt = ParseStmt();
    if (stmt == nullptr) {
      return nullptr;
    }
    stmts.push_back(stmt);
  }

  NextToken();
  return new Block{{Stmt::Block}, stmts};
}

Stmt* Parser::ParseIfElse() {
  NextToken();

  if (!ExpectChar('(')) return nullptr;
  auto* cond = ParseExpr();
  if (!ExpectChar(')')) return nullptr;

  auto* then = ParseStmt();
  if (then == nullptr) return nullptr;

  Stmt* else_then = nullptr;
  if (IsTokenKeyword(Keyword::Else)) {
    NextToken();
    else_then = ParseStmt();
    if (else_then == nullptr) return nullptr;
  }
  return new If{{Stmt::If}, cond, then, else_then};
}

Stmt* Parser::ParseWhile() {
  NextToken();

  if (!ExpectChar('(')) return nullptr;
  auto* cond = ParseExpr();
  if (!ExpectChar(')')) return nullptr;

  auto* body = ParseStmt();
  if (body == nullptr) return nullptr;

  return new While{{Stmt::While}, cond, body};
}

Expr* Parser::ParseExpr() {
  std::stack<Expr*> oprs;
  std::stack<Operator> ops;

  auto* expr = ParseUnary();
  if (expr == nullptr) {
    return nullptr;
  }

  oprs.push(expr);
  while (cur_token_ == Token::Operator) {
    auto op = lexer_.OpVal();
    if (GetOpPrec(op) <= 0) {
      break;
    }

    NextToken();
    while (!ops.empty() && GetOpPrec(ops.top()) >= GetOpPrec(op)) {
      auto cur_op = GetBinaryOp(ops.top());
      ops.pop();
      auto* rhs = oprs.top();
      oprs.pop();
      auto* lhs = oprs.top();
      oprs.pop();
      oprs.push(new Binary{{cur_op, 0}, lhs, rhs});
    }
    ops.push(op);
    expr = ParseUnary();
    if (expr == nullptr) return nullptr;
    oprs.push(expr);
  }

  while (!ops.empty()) {
    auto cur_op = GetBinaryOp(ops.top());
    ops.pop();
    auto* rhs = oprs.top();
    oprs.pop();
    auto* lhs = oprs.top();
    oprs.pop();
    oprs.push(new Binary{{cur_op, 0}, lhs, rhs});
  }
  return oprs.top();
}

Expr* Parser::ParseUnary() {
  if (cur_token_ == Token::Operator) {
    switch (lexer_.OpVal()) {
      case Operator::Add:
        NextToken();
        return ParseUnary();
      case Operator::Sub:
        NextToken();
        return new Binary{
            {Expr::Sub, 0}, new IntConst{{Expr::IntConst, 0}, 0}, ParseUnary()};
      case Operator::LogicNot:
        NextToken();
        return new Binary{
            {Expr::Eq, 0}, new IntConst{{Expr::IntConst, 0}, 0}, ParseUnary()};
      default:
        spdlog::error("Parser error: expect unary operator");
        return nullptr;
    }
  }
  return ParseFactor();
}

Expr* Parser::ParseFactor() {
  Expr* expr = nullptr;
  if (IsTokenChar('(')) {
    NextToken();
    expr = ParseExpr();
    if (!ExpectChar(')')) return nullptr;
    return expr;
  }

  if (cur_token_ == Token::Id) {
    std::string id = lexer_.IdVal();
    NextToken();

    if (IsTokenChar('(')) {
      auto args = ParseExprList();
      return new Call{id, args};
    }

    auto dims = ParseArrayDims();
    return new Index{{Expr::Index, 0}, id, dims};
  }

  return ParseIntConst();
}

Expr* Parser::ParseIntConst() {
  Expr* expr = nullptr;
  if (cur_token_ == Token::Int) {
    expr = new IntConst{{Expr::IntConst, 0}, static_cast<i32>(lexer_.IntVal())};

    NextToken();
    return expr;
  }
  spdlog::error("Parser error: expect integer constant");
  return nullptr;
}

std::vector<Expr*> Parser::ParseExprList() {
  NextToken();
  std::vector<Expr*> args;

  if (!IsTokenChar(')')) {
    for (;;) {
      auto* arg = ParseExpr();
      if (arg == nullptr) {
        return {};
      }
      args.push_back(arg);

      if (!IsTokenChar(',')) {
        break;
      }
      NextToken();
    }
  }

  if (!ExpectChar(')')) return {};

  return args;
}

std::vector<Expr*> Parser::ParseArrayDims() {
  std::vector<Expr*> dims;

  while (IsTokenChar('[')) {
    NextToken();

    auto* dim = ParseExpr();
    if (dim == nullptr) {
      return {};
    }
    dims.push_back(dim);

    if (!ExpectChar(']')) {
      return {};
    }
  }
  return dims;
}

std::vector<Expr*> Parser::ParseArrayDims0() {
  NextToken();
  if (!ExpectChar(']')) {
    return {};
  }

  std::vector<Expr*> dims{nullptr};
  while (IsTokenChar('[')) {
    NextToken();

    auto* dim = ParseExpr();
    if (dim == nullptr) {
      return {};
    }
    dims.push_back(dim);

    if (!ExpectChar(']')) {
      return {};
    }
  }
  return dims;
}

bool Parser::ExpectChar(char c) {
  if (!IsTokenChar(c)) {
    spdlog::error("Parser error: expected '{}'", c);
    return false;
  }
  NextToken();
  return true;
}

bool Parser::ExpectId() {
  if (cur_token_ != Token::Id) {
    spdlog::error("Parser error: expected identifier");
    return false;
  }
  return true;
}