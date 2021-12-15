#pragma once

#include "string_view.h"
#include "lex.h"
#include "expr.h"

class parser {
public:
  explicit parser(string_view input);
  token& peek() { return m_lexer.peek();}
  void skip() { m_lexer.skip(); }
  Expr parse_program();
  Expr parse_head();
  Expr parse_rest();
  Expr parse_atom();
  Expr parse_subexpr();

 private:
  lexer m_lexer;
};
