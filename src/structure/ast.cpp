#include "structure/ast.hpp"

IntConst IntConst::ZERO{Expr::IntConst, 0, 0};
Break Break::INSTANCE{Stmt::Break};
Continue Continue::INSTANCE{Stmt::Continue};

Func Func::BUILTIN[9] = {
    Func{true, "getint"},
    Func{true, "getch"},
    Func{true, "getarray", {Decl{false, false, false, "a", {nullptr}}}},
    Func{false, "putint", {Decl{false, false, false, "n"}}},
    Func{false, "putch", {Decl{false, false, false, "c"}}},
    Func{false,
         "putarray",
         {Decl{false, false, false, "n"},
          Decl{false, false, false, "a", {nullptr}}}},
    Func{false, "starttime"},
    Func{false, "stoptime"},
    Func{false,
         "memset",
         {Decl{false, false, false, "arr", {nullptr}},
          Decl{false, false, false, "num"},
          Decl{false, false, false, "count"}}}};
