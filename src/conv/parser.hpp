#pragma once

#include <iostream>
#include <string>
#include <string_view>

#include "../structure/ast.hpp"
#include "lexer.hpp"
#include "token.hpp"

class Parser {
 public:
  Parser(Lexer &lexer) : lexer_(lexer) { Reset(); }

  // reset parser status
  void Reset() {
    lexer_.Reset();
    ended_ = false;
    NextToken();
  }

  // getters
  // returns true if parser met EOF
  bool ended() const { return ended_; }

  Program ParseProgram();
  Func ParseFunction(Keyword ret_type, std::string id);
  Decl ParseParam();
  std::vector<Decl> ParseDecl(bool is_const, std::string first_id);
  InitList ParseInitList();

  Stmt *ParseStmt();
  Stmt *ParseEmptyStmt();
  Stmt *ParseBare();
  Stmt *ParseBlock();
  Stmt *ParseIfElse();
  Stmt *ParseWhile();

  Expr *ParseExpr();
  Expr *ParseUnary();
  Expr *ParseFactor();
  Expr *ParseIntConst();

  std::vector<Expr *> ParseExprList();
  std::vector<Expr *> ParseArrayDims();
  std::vector<Expr *> ParseArrayDims0();

 private:
  // get next token from lexer and skip all EOLs
  Token NextToken() {
    cur_token_ = lexer_.NextToken();
    return cur_token_;
  }

  // check if current token is a character (token type 'Other')
  bool IsTokenChar(char c) const {
    return (cur_token_ == Token::Other && lexer_.other_val() == c) ||
           (cur_token_ == Token::Id && lexer_.id_val().size() == 1 && lexer_.id_val()[0] == c);
  }

  // check if current token is a keyword
  bool IsTokenKeyword(Keyword key) const { return cur_token_ == Token::Keyword && lexer_.key_val() == key; }

  // check if current token is an operator
  bool IsTokenOperator(Operator op) const { return cur_token_ == Token::Operator && lexer_.op_val() == op; }

  // make sure current token is specific character and goto next token
  bool ExpectChar(char c);
  // make sure current token is identifier
  bool ExpectId();

  Lexer &lexer_;
  Token cur_token_;
  bool ended_;
};
