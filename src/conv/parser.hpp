#pragma once

#include <string>

#include "conv/lexer.hpp"
#include "conv/token.hpp"
#include "structure/ast.hpp"

class Parser {
 public:
  explicit Parser(Lexer& lexer) : lexer_(lexer) { Reset(); }

  // reset parser status
  void Reset() {
    lexer_.Reset();
    NextToken();
  }

  Program ParseProgram();
  Func ParseFunction(Keyword ret_type, const std::string& id);
  Decl ParseParam();
  std::vector<Decl> ParseDecl(bool is_const, const std::string& first_id);
  InitList ParseInitList();

  Stmt* ParseStmt();
  Stmt* ParseEmptyStmt();
  Stmt* ParseBare();
  Stmt* ParseBlock();
  Stmt* ParseIfElse();
  Stmt* ParseWhile();

  Expr* ParseExpr();
  Expr* ParseUnary();
  Expr* ParseFactor();
  Expr* ParseIntConst();

  std::vector<Expr*> ParseExprList();
  std::vector<Expr*> ParseArrayDims();
  std::vector<Expr*> ParseArrayDims0();

 private:
  // get next token from lexer and skip all EOLs
  Token NextToken() {
    cur_token_ = lexer_.NextToken();
    return cur_token_;
  }

  // check if current token is a character (token type 'Other')
  [[nodiscard]] bool IsTokenChar(char c) const {
    return (cur_token_ == Token::Other && lexer_.OtherVal() == c) ||
           (cur_token_ == Token::Id && lexer_.IdVal().size() == 1 &&
            lexer_.IdVal()[0] == c);
  }

  // check if current token is a keyword
  [[nodiscard]] bool IsTokenKeyword(Keyword key) const {
    return cur_token_ == Token::Keyword && lexer_.KeyVal() == key;
  }

  // check if current token is an operator
  [[nodiscard]] bool IsTokenOperator(Operator op) const {
    return cur_token_ == Token::Operator && lexer_.OpVal() == op;
  }

  // make sure current token is specific character and goto next token
  bool ExpectChar(char c);
  // make sure current token is identifier
  bool ExpectId();

  Lexer& lexer_;
  Token cur_token_;
};
