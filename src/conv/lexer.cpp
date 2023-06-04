#include "lexer.hpp"

#include <cctype>
#include <charconv>
#include <cstddef>
#include <cstdlib>
#include <cstring>

namespace {

enum class NumberType {
  Normal = 10,
  Hex = 16,
  Oct = 8,
};

// get index of a string in string array
template <typename T, std::size_t N>
int GetIndex(const char *str, T (&str_array)[N]) {
  for (std::size_t i = 0; i < N; ++i) {
    if (!std::strcmp(str, str_array[i])) return i;
  }
  return -1;
}

bool IsOperatorHeadChar(char c) {
  const char op_head_chars[] = "+-*/%=!<>&|~^.";
  for (const auto &i : op_head_chars) {
    if (i == c) return true;
  }
  return false;
}

bool IsOperatorChar(char c) {
  const char op_chars[] = "=&|<>";
  for (const auto &i : op_chars) {
    if (i == c) return true;
  }
  return false;
}

}  // namespace

Token Lexer::LogError(std::string_view message) {
  std::cerr << "Lexer error: " << message << '\n';
  return Token::Error;
}

void Lexer::SkipSpaces() {
  while (!IsEOL() && std::isspace(last_char_)) NextChar();
}

Token Lexer::HandleId() {
  // read string
  std::string id;
  do {
    id += last_char_;
    NextChar();
  } while (!IsEOL() && (std::isalnum(last_char_) || last_char_ == '_'));
  // check if string is keyword
  int index = GetIndex(id.c_str(), kKeywords);
  if (index < 0) {
    id_val_ = id;
    return Token::Id;
  } else {
    key_val_ = static_cast<Keyword>(index);
    return Token::Keyword;
  }
}

Token Lexer::HandleNum() {
  std::string num;
  NumberType num_type = NumberType::Normal;
  // check if is hexadecimal/octal number
  if (last_char_ == '0') {
    NextChar();
    if (last_char_ == 'x' || last_char_ == 'X') {
      // hexadecimal
      num_type = NumberType::Hex;
      NextChar();
    } else {
      // octal (also treat zero as octal)
      num_type = NumberType::Oct;
      // append the leading '0'
      num += '0';
    }
  }
  // read number string
  while (!IsEOL() && std::isxdigit(last_char_)) {
    num += last_char_;
    NextChar();
  }
  // convert to number
  auto [ptr, ec] = std::from_chars(num.data(), num.data() + num.size(), int_val_, static_cast<int>(num_type));
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
  } else {
    op_val_ = static_cast<Operator>(index);
    return Token::Operator;
  }
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
  if (!in_) return;
  // reset status of file stream
  in_->clear();
  in_->seekg(0, std::ios::beg);
  *in_ >> std::noskipws;
}

void Lexer::Reset(std::istream *in) {
  in_ = in;
  Reset();
}

Token Lexer::NextToken() {
  // end of file
  if (IsEOF()) return Token::End;
  // skip spaces
  SkipSpaces();
  // id or keyword
  if (std::isalpha(last_char_) || last_char_ == '_') return HandleId();
  // number
  if (std::isdigit(last_char_)) return HandleNum();
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
        std::cout << "token{End}\n";
        break;
      case Token::Id:
        std::cout << "token{Id}: " << id_val_ << '\n';
        break;
      case Token::Int:
        std::cout << "token{Int}: " << int_val_ << '\n';
        break;
      case Token::Keyword:
        std::cout << "token{keyword}: " << kKeywords[static_cast<int>(key_val_)] << '\n';
        break;
      case Token::Operator:
        std::cout << "token{operator}: " << kOperators[static_cast<int>(op_val_)] << '\n';
        break;
      case Token::Other:
        std::cout << "token{other}: " << other_val_ << '\n';
        break;
      default:
        break;
    }
  }
}