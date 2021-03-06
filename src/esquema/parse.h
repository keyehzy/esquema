#pragma once

#include "expr.h"
#include "lex.h"
#include "string_view.h"

class parser {
 public:
  explicit parser(string_view input);
  token& peek() { return m_lexer.peek(); }
  void skip() { m_lexer.skip(); }
  Expr parse_program();
  Expr parse_head();
  Expr parse_rest();
  Expr parse_atom();
  Expr parse_subexpr();
  Expr parse_single_token(Expr_kind);
  Expr qq_expand(Expr);
  Expr qq_expand_list(Expr);
  Expr append(Expr, Expr);
  Expr make_list2(Expr, Expr);
  Expr make_list3(Expr, Expr, Expr);

 private:
  lexer m_lexer;
};
