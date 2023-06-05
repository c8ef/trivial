#pragma once

#include <cstdint>
#include <istream>
#include <string>
#include <string_view>

#include "conv/token.hpp"

inline constexpr const char* kKeywords[] = {
    MIMIC_KEYWORDS(MIMIC_EXPAND_SECOND)};
inline constexpr const char* kOperators[] = {
    MIMIC_OPERATORS(MIMIC_EXPAND_SECOND)};

class Lexer {
 public:
  Lexer() { Reset(nullptr); }
  explicit Lexer(std::istream* in) { Reset(in); }

  // reset lexer status
  void Reset();
  // reset lexer status (including input stream)
  void Reset(std::istream* in);
  // get next token from input stream
  Token NextToken();
  // dump the lexer output
  void DumpTokens();

  // identifiers
  const std::string& id_val() const { return id_val_; }
  // integer values
  std::int64_t int_val() const { return int_val_; }
  // keywords
  Keyword key_val() const { return key_val_; }
  // operators
  Operator op_val() const { return op_val_; }
  // other characters
  char other_val() const { return other_val_; }

 private:
  bool IsEOF() { return (in_ == nullptr) || in_->eof(); }
  bool IsEOL() { return IsEOF() || last_char_ == '\n' || last_char_ == '\r'; }
  void NextChar() {
    if (IsEOF()) return;
    *in_ >> last_char_;
  }

  // print error message and return Token::Error
  Token LogError(std::string_view message);

  // skip spaces in stream
  void SkipSpaces();

  Token HandleId();
  Token HandleNum();
  Token HandleOperator();
  Token HandleComment();
  Token HandleBlockComment();
  Token HandleEOL();

  std::istream* in_;
  char last_char_;
  // value of token
  std::string id_val_;
  std::int64_t int_val_;
  Keyword key_val_;
  Operator op_val_;
  char other_val_;
};
