#pragma once

#include "token.h"
#include <vector>

class Atom {
public:
  Atom() {};
  Atom(token tok) : m_token(tok) {};
  Atom(token_t type, location loc) : m_token(token(type, loc)) {};
  Atom(token_t type, string_view sv) : m_token(token(type, sv)) {};
  Atom(int i) : is_evaluated(true), m_token(token(token_t::integer)), m_value_int(i) {};
  Atom(double f) : is_evaluated(true), m_token(token(token_t::float_)), m_value_float(f) {};

  token token_() const { return m_token; }
  token_t type() const { return m_token.type; }
  string_view as_string() { return m_token.as_string(); }
  int as_int() { return m_value_int; }
  double as_float() { return m_value_float; }

  bool operator==(const Atom& a) {
    return m_token.type == a.token_().type &&
    m_token.as_string() == a.token_().as_string();
  }

  bool is_evaluated = false;

private:
  token m_token;
  int m_value_int;
  double m_value_float;
};

enum Expr_kind {
  err,
  nil,
  atom,
  cons,

  procedure,
  begin,
  quote,
  if_,
  false_,
  set,
  true_,
};

class Cons;

class Expr {
public:
  Expr() {};
  explicit Expr(Atom atom) : m_kind(Expr_kind::atom), m_atom(atom) {};
  explicit Expr(Cons* cons) : m_kind(Expr_kind::cons), m_cons(cons) {};
  explicit Expr(Expr_kind kind, Atom atom) : m_kind(kind), m_atom(atom) {};
  explicit Expr(Expr_kind kind, Cons* cons) : m_kind(kind), m_cons(cons) {};
  explicit Expr(Expr (*func) (std::vector<Expr>)) : m_func(func), m_kind(Expr_kind::procedure) {};
  explicit Expr(Expr_kind kind) : m_kind(kind) {};

  Expr_kind kind () const { return m_kind; }
  Atom atom () const { return m_atom; }
  Cons *cons () const { return m_cons; }

  static Expr nil() { return Expr(Expr_kind::nil); }
  static Expr err() { return Expr(Expr_kind::err); }
  Expr (*m_func)(std::vector<Expr>);

private:
  Expr_kind m_kind;
  Atom m_atom;
  Cons *m_cons;
};

class Cons {
public:
  Cons() = delete;

  static Cons* cons(Expr head, Expr rest) {
    Cons *cons = (Cons*)malloc(sizeof(Cons));
    cons->car = head;
    cons->cdr = rest;
    return cons;
  }

  Expr car;
  Expr cdr;
};
