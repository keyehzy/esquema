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
  Expr invoke(Expr, std::vector<Expr>);
  std::vector<Expr> eval_list(Expr);
  Expr bind_variable_in_current_env(Atom, Expr);

 private:
  Env m_initial_env;
  Env m_extended_env;
  Expr m_value;
  Expr m_original_value;
  parser m_parser;
};

inline Expr CAR(Expr x) { return x.cons()->car; }
inline Expr CADR(Expr x) { return x.cons()->cdr.cons()->car; }
inline Expr CAADR(Expr x) { return x.cons()->cdr.cons()->car.cons()->car; }
inline Expr CADDR(Expr x) { return x.cons()->cdr.cons()->cdr.cons()->car; }
inline Expr CADDDR(Expr x) {
  return x.cons()->cdr.cons()->cdr.cons()->cdr.cons()->car;
}

inline Expr CDR(Expr x) { return x.cons()->cdr; }
inline Expr CDDR(Expr x) { return x.cons()->cdr.cons()->cdr; }
inline Expr CDAR(Expr x) { return x.cons()->car.cons()->cdr; }
inline Expr CDADR(Expr x) { return x.cons()->cdr.cons()->car.cons()->cdr; }
