#pragma once

#include "expr.h"
#include "parse.h"
#include "string_view.h"
#include <vector>

class evaluator {
 public:
  explicit evaluator(string_view input);
  void extend_env(Atom, Expr);
  void populate_env();
  Expr eval(Expr);
  Expr eval_atom(Expr);
  Expr eval_symbol(Expr);
  Expr eprogn(Expr);
  Expr update(Expr, Expr);
  Expr value() const;
  Expr invoke(Expr, std::vector<Expr>);
  std::vector<Expr> eval_list(Expr);

  static Expr my_plus(Expr);

 private:
  std::vector<symbol_value> m_env;
  Expr m_value;
  Expr m_original_value;
  parser m_parser;
};

inline Expr CAR(Expr x) { return x.cons()->car; }

inline Expr CDR(Expr x) { return x.cons()->cdr; }

inline Expr CADR(Expr x) { return x.cons()->cdr.cons()->car; }

inline Expr CADDR(Expr x) { return x.cons()->cdr.cons()->cdr.cons()->car; }

inline Expr CADDDR(Expr x) {
  return x.cons()->cdr.cons()->cdr.cons()->cdr.cons()->car;
}
