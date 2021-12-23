#pragma once

#include "expr.h"
#include "parse.h"
#include "string_view.h"
#include <optional>
#include <vector>

class evaluator {
 public:
  explicit evaluator(string_view input);
  void populate_env();
  Expr eval(Expr, Env&);
  Expr eval_atom(Expr, Env&);
  Expr eval_define(Expr, Env&);
  Expr lookup_symbol(Expr, Env);
  Expr eprogn(Expr, Env&);
  Expr set(Atom, Expr, Env&);
  Expr value() const;
  Expr invoke(Expr, List);
  List eval_list(Expr, Env&);
  Expr bind_variable(Expr, Expr, Env&);
  Expr build_list(Expr, Env&);
  Expr append_list(Expr, Expr, Env&);

  bool is_at_top_level(const Env&);
  bool is_inside_lambda = false;

 private:
  Env m_protected_env;
  Env m_toplevel_env;
  Expr m_value;
  Expr m_original_value;
  parser m_parser;
};
