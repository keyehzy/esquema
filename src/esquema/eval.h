#pragma once

#include "parse.h"
#include "string_view.h"
#include <vector>

struct symbol_value {
  Atom symbol;
  Expr value;
};

typedef Expr (*FN) (std::vector<Atom>);

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
  std::vector<Expr> flatten(Expr);


  static Expr my_plus(Expr);

private:
  std::vector<symbol_value> m_env;
  Expr m_value;
  Expr m_original_value;
  parser m_parser;
};

#define CAR(x) (x).cons()->car
#define CDR(x) (x).cons()->cdr
#define CADR(x) (x).cons()->cdr.cons()->car
#define CADDR(x) (x).cons()->cdr.cons()->cdr.cons()->car
#define CADDDR(x) (x).cons()->cdr.cons()->cdr.cons()->cdr.cons()->car
