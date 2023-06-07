#pragma once

// all supported keywords
#define TRIVIAL_KEYWORDS(e)                                         \
  e(If, "if") e(Else, "else") e(While, "while") e(Break, "break") \
      e(Continue, "continue") e(Return, "return") e(Void, "void") \
          e(Int, "int") e(Const, "const")

// all supported operators
#define TRIVIAL_OPERATORS(e)                                          \
  e(Add, "+", 90) e(Sub, "-", 90) e(Mul, "*", 100) e(Div, "/", 100) \
      e(Mod, "%", 100) e(Equal, "==", 60) e(NotEqual, "!=", 60)     \
          e(Less, "<", 70) e(LessEqual, "<=", 70) e(Great, ">", 70) \
              e(GreatEqual, ">=", 70) e(LogicAnd, "&&", 20)         \
                  e(LogicOr, "||", 10) e(LogicNot, "!", -1) e(Assign, "=", 0)

// expand first element to comma-separated list
#define TRIVIAL_EXPAND_FIRST(i, ...) i,
// expand second element to comma-separated list
#define TRIVIAL_EXPAND_SECOND(i, j, ...) j,
// expand third element to comma-separated list
#define TRIVIAL_EXPAND_THIRD(i, j, k, ...) k,

enum class Token {
  Error,
  End,
  Id,
  Int,
  Keyword,
  Operator,
  Other,
};

enum class Keyword { TRIVIAL_KEYWORDS(TRIVIAL_EXPAND_FIRST) };
enum class Operator { TRIVIAL_OPERATORS(TRIVIAL_EXPAND_FIRST) };
