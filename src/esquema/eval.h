#pragma once

#include "expr.h"
#include "parse.h"
#include "string_view.h"

class evaluator {
 public:
  explicit evaluator(string_view input);
  void populate_env();
  Expr eval(const Expr&, Env&);
  Expr eval_atom(const Expr&, Env&);
  Expr eval_define(const Expr&, Env&);
  Expr eval_let(const Expr&, Env&);
  Expr eval_letrec(const Expr&, Env&);
  Expr eval_syntactic_keyword(const Expr&, Env&);
  Expr lookup_symbol(const Expr&, const Env&);
  Expr eprogn(const Expr&, Env&);
  Expr set(const Atom&, const Expr&, Env&);
  Expr value() const;
  Expr invoke(const Expr&, const List&);
  List eval_list(const Expr&, Env&);
  Expr bind_variable(const Expr&, const Expr&, Env&);
  Expr build_list(const Expr&, Env&);
  Expr append_list(const Expr&, const Expr&, Env&);
  bool equal(const Expr&, const Expr&);

  bool is_at_top_level(const Env&);
  bool is_inside_lambda = false;

 private:
  Env m_protected_env;
  Env m_toplevel_env;
  Expr m_value;
  Expr m_original_value;
  parser m_parser;
};
