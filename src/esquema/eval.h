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
  Expr variables_from_init_list(Expr);
  Expr inits_from_init_list(Expr);

  bool is_at_top_level(const Env&);
  bool is_inside_lambda = false;

 private:
  Env m_protected_env;
  Env m_toplevel_env;
  Expr m_value;
  Expr m_original_value;
  parser m_parser;
};

inline Expr CAR(const Expr& x) { return x.cons()->car; }
inline Expr CAAR(const Expr& x) { return x.cons()->car.cons()->car; }
inline Expr CADR(const Expr& x) { return x.cons()->cdr.cons()->car; }
inline Expr CADAR(const Expr& x) {
  return x.cons()->car.cons()->cdr.cons()->car;
}
inline Expr CAADR(const Expr& x) {
  return x.cons()->cdr.cons()->car.cons()->car;
}
inline Expr CADDR(const Expr& x) {
  return x.cons()->cdr.cons()->cdr.cons()->car;
}
inline Expr CADDDR(const Expr& x) {
  return x.cons()->cdr.cons()->cdr.cons()->cdr.cons()->car;
}

inline Expr CDR(const Expr& x) { return x.cons()->cdr; }
inline Expr CDDR(const Expr& x) { return x.cons()->cdr.cons()->cdr; }
inline Expr CDAR(const Expr& x) { return x.cons()->car.cons()->cdr; }
inline Expr CDADR(const Expr& x) {
  return x.cons()->cdr.cons()->car.cons()->cdr;
}
