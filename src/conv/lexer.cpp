#include "conv/lexer.hpp"

#include <algorithm>
#include <cctype>
#include <charconv>
#include <cstddef>
#include <cstdlib>
#include <cstring>

#include "common.hpp"

namespace {

enum class NumberType {
  Normal = 10,
  Hex = 16,
  Oct = 8,
};

// get index of a string in string array
template <typename T, std::size_t N>
int GetIndex(const char* str, T (&str_array)[N]) {
  for (std::size_t i = 0; i < N; ++i) {
    if (!std::strcmp(str, str_array[i])) return i;
  }
  return -1;
}

bool IsOperatorHeadChar(char c) {
  const char op_head_chars[] = "+-*/%=!<>&|~^.";
  return std::any_of(std::begin(op_head_chars), std::end(op_head_chars),
                     [=](char i) -> bool { return i == c; });
}

bool IsOperatorChar(char c) {
  const char op_chars[] = "=&|<>";
  return std::any_of(std::begin(op_chars), std::end(op_chars),
                     [=](char i) -> bool { return i == c; });
}

}  // namespace

Token Lexer::LogError(std::string_view message) {
  spdlog::error("Lexer error: {}", message);
  return Token::Error;
}

void Lexer::SkipSpaces() {
  while (!IsEOL() && (isspace(last_char_) != 0)) NextChar();
}

Token Lexer::HandleId() {
  // read string
  std::string id;
  do {
    id += last_char_;
    NextChar();
  } while (!IsEOL() && ((isalnum(last_char_) != 0) || last_char_ == '_'));
  // check if string is keyword
  int index = GetIndex(id.c_str(), kKeywords);
  if (index < 0) {
    id_val_ = id;
    return Token::Id;
  }
  key_val_ = static_cast<Keyword>(index);
  return Token::Keyword;
}

Token Lexer::HandleNum() {
  std::string num;
  NumberType num_type = NumberType::Normal;
  // check if is hex/oct number
  if (last_char_ == '0') {
    NextChar();
    if (last_char_ == 'x' || last_char_ == 'X') {
      num_type = NumberType::Hex;
      NextChar();
    } else {
      num_type = NumberType::Oct;
      num += '0';
    }
  }
  // read number string
  while (!IsEOL() && (isxdigit(last_char_) != 0)) {
    num += last_char_;
    NextChar();
  }
  // convert to number
  auto [ptr, ec] = std::from_chars(num.data(), num.data() + num.size(),
                                   int_val_, static_cast<int>(num_type));
  // check if conversion is valid
  return ec == std::errc() ? Token::Int : LogError("invalid number literal");
}

Token Lexer::HandleOperator() {
  std::string op;
  // read first char
  op += last_char_;
  NextChar();
  // check if is comment
  if (op[0] == '/' && !IsEOL()) {
    switch (last_char_) {
      case '/':
        return HandleComment();
      case '*':
        return HandleBlockComment();
    }
  }
  // read rest chars
  while (!IsEOL() && IsOperatorChar(last_char_)) {
    op += last_char_;
    NextChar();
  }
  // check if operator is valid
  int index = GetIndex(op.c_str(), kOperators);
  if (index < 0) {
    return LogError("unknown operator");
  }
  op_val_ = static_cast<Operator>(index);
  return Token::Operator;
}

Token Lexer::HandleComment() {
  // eat '/'
  NextChar();
  while (!IsEOL()) NextChar();
  return NextToken();
}

Token Lexer::HandleBlockComment() {
  // eat '*'
  NextChar();
  // read until there is '*/' in stream
  bool star = false;
  while (!IsEOF() && !(star && last_char_ == '/')) {
    star = last_char_ == '*';
    NextChar();
  }
  // check unclosed block comment
  if (IsEOF()) return LogError("comment unclosed at EOF");
  // eat '/'
  NextChar();
  return NextToken();
}

Token Lexer::HandleEOL() {
  do {
    NextChar();
  } while (IsEOL() && !IsEOF());
  return NextToken();
}

void Lexer::Reset() {
  last_char_ = ' ';
  if (in_ == nullptr) return;
  // reset status of file stream
  in_->clear();
  in_->seekg(0, std::ios::beg);
  *in_ >> std::noskipws;
}

void Lexer::Reset(std::istream* in) {
  in_ = in;
  Reset();
}

Token Lexer::NextToken() {
  // end of file
  if (IsEOF()) return Token::End;
  // skip spaces
  SkipSpaces();
  // id or keyword
  if ((isalpha(last_char_) != 0) || last_char_ == '_') return HandleId();
  // number
  if (isdigit(last_char_) != 0) return HandleNum();
  // operator or id
  if (IsOperatorHeadChar(last_char_)) return HandleOperator();
  // end of line (line break or delimiter)
  if (IsEOL()) return HandleEOL();
  // other characters
  other_val_ = last_char_;
  NextChar();
  return Token::Other;
}

void Lexer::DumpTokens() {
  Token cur_token;
  while ((cur_token = NextToken()) != Token::End) {
    switch (cur_token) {
      case Token::End:
        spdlog::info("token[End]");
        break;
      case Token::Id:
        spdlog::info("token[Id]: {}", id_val_);
        break;
      case Token::Int:
        spdlog::info("token[Int]: {}", int_val_);
        break;
      case Token::Keyword:
        spdlog::info("token[Keyword]: {}",
                     kKeywords[static_cast<int>(key_val_)]);
        break;
      case Token::Operator:
        spdlog::info("token[Operator]: {}",
                     kOperators[static_cast<int>(op_val_)]);
        break;
      case Token::Other:
        spdlog::info("token[Other]: {}", other_val_);
        break;
      default:
        break;
    }
  }
}