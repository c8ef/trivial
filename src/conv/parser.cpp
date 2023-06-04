#include "parser.hpp"

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

Stmt *Parser::ParseStmt() {
  if (IsTokenChar('{')) {
    return ParseBlock();
  }

  if (IsTokenChar(';')) {
    return ParseEmptyStmt();
  }
  return ParseBare();
}

Stmt *Parser::ParseEmptyStmt() {
  NextToken();
  return new ExprStmt{Stmt::ExprStmt, nullptr};
}

Stmt *Parser::ParseBare() {
  auto *expr = ParseExpr();
  if (!expr) {
    return nullptr;
  }
  if (!ExpectChar(';')) {
    return nullptr;
  }
  return new ExprStmt{Stmt::ExprStmt, expr};
}

Stmt *Parser::ParseBlock() {
  if (!ExpectChar('{')) {
    return nullptr;
  }

  std::vector<Stmt *> stmts;
  while (!IsTokenChar('}')) {
    auto *stmt = ParseStmt();
    if (!stmt) {
      return nullptr;
    }
    stmts.push_back(stmt);
  }

  NextToken();
  return new Block{Stmt::Block, stmts};
}

Expr *Parser::ParseExpr() {
  std::stack<Expr *> oprs;
  std::stack<Operator> ops;

  auto *expr = ParseUnary();
  if (expr == nullptr) {
    return nullptr;
  }

  oprs.push(expr);
  while (cur_token_ == Token::Operator) {
    auto op = lexer_.op_val();
    if (GetOpPrec(op) <= 0) {
      break;
    }

    NextToken();
    while (!ops.empty() && GetOpPrec(ops.top()) >= GetOpPrec(op)) {
      auto cur_op = GetBinaryOp(ops.top());
      ops.pop();
      auto rhs = oprs.top();
      oprs.pop();
      auto lhs = oprs.top();
      oprs.pop();
      oprs.push(new Binary{cur_op, 0, lhs, rhs});
    }
    ops.push(op);
    expr = ParseUnary();
    if (!expr) return nullptr;
    oprs.push(expr);
  }

  while (!ops.empty()) {
    auto cur_op = GetBinaryOp(ops.top());
    ops.pop();
    auto rhs = oprs.top();
    oprs.pop();
    auto lhs = oprs.top();
    oprs.pop();
    oprs.push(new Binary{cur_op, 0, lhs, rhs});
  }
  return oprs.top();
}

Expr *Parser::ParseUnary() {
  if (cur_token_ == Token::Operator) {
    switch (lexer_.op_val()) {
      case Operator::Add:
        NextToken();
        return ParseFactor();
      case Operator::Sub:
        NextToken();
        return new Binary{Expr::Sub, 0, &IntConst::ZERO, ParseFactor()};
      case Operator::LogicNot:
        NextToken();
        return new Binary{Expr::Eq, 0, &IntConst::ZERO, ParseFactor()};
      default:
        std::cerr << "expect unary operator\n";
        return nullptr;
    }
  }
  return ParseFactor();
}

Expr *Parser::ParseFactor() {
  Expr *expr = nullptr;
  if (IsTokenChar('(')) {
    NextToken();
    expr = ParseExpr();
    if (!ExpectChar(')')) {
      std::cerr << "brace doesn't match\n";
      return nullptr;
    }
    return expr;
  }

  if (cur_token_ == Token::Id) {
    std::string id = lexer_.id_val();
    NextToken();

    if (IsTokenChar('(')) {
      auto args = ParseExprList();
      return new Call{id, args, 0};
    }

    if (IsTokenChar('[')) {
      auto dims = ParseArrayDims();
      return new Index{Expr::Index, 0, id, dims};
    }
  }

  return ParseIntConst();
}

Expr *Parser::ParseIntConst() {
  Expr *expr = nullptr;
  if (cur_token_ == Token::Int) {
    expr = new IntConst{Expr::IntConst, 0, static_cast<i32>(lexer_.int_val())};

    NextToken();
    return expr;
  }
  std::cerr << "expect integer constant\n";
  assert(false);
}

std::vector<Expr *> Parser::ParseExprList() {
  NextToken();
  std::vector<Expr *> args;

  if (!IsTokenChar(')')) {
    for (;;) {
      auto *arg = ParseExpr();
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

  if (!ExpectChar(')')) {
    return {};
  }
  return args;
}

std::vector<Expr *> Parser::ParseArrayDims() {
  std::vector<Expr *> dims;

  while (IsTokenChar('[')) {
    NextToken();

    auto *dim = ParseExpr();
    if (dim == nullptr) {
      return {};
    }
    dims.push_back(dim);

    if (!IsTokenChar(']')) {
      std::cerr << "expect ']'\n";
      return {};
    }
    NextToken();
  }
  return dims;
}

bool Parser::ExpectChar(char c) {
  if (!IsTokenChar(c)) {
    std::string msg = "expected '";
    msg = msg + c + "'";
    std::cerr << msg << '\n';
    return false;
  }
  NextToken();
  return true;
}

bool Parser::ExpectId() {
  if (cur_token_ != Token::Id) {
    std::cerr << "expected identifier\n";
    return false;
  }
  return true;
}