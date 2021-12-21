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
  Expr eval(Expr);
  Expr eval_atom(Expr);
  Expr eval_define(Expr);
  Expr lookup_symbol(Expr);
  Expr eprogn(Expr);
  Expr set(Atom, Expr);
  Expr value() const;
  Expr invoke(Expr, List);
  List eval_list(Expr);
  Expr bind_variable_in_current_env(Expr, Expr);
  Expr bind_variable_to_env(Expr, Expr, Env&);
  Expr variables_from_init_list(Expr);
  Expr inits_from_init_list(Expr);

  Env* get_scope();
  void push_scope(Env*);
  void pop_scope();

 private:
  Env m_env{};
  Env m_protected_env{};
  std::vector<Env*> m_scope_collection;
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
