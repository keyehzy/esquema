#pragma once

#include "token.h"
#include <vector>

class Atom {
 public:
  Atom(){};
  Atom(token tok) : m_token(tok){};
  Atom(token_t type, location loc) : m_token(token(type, loc)){};
  Atom(token_t type, string_view sv) : m_token(token(type, sv)){};
  Atom(string_view sv) : m_token(token(token_t::symbol, sv)){};
  Atom(int i)
      : is_evaluated(true), m_token(token(token_t::integer)), m_value_int(i){};
  Atom(double f)
      : is_evaluated(true), m_token(token(token_t::float_)), m_value_float(f){};

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

  begin,
  define,
  function,
  false_,
  if_,
  lambda,
  named_lambda,
  procedure,
  quote,
  set,
  true_,
};

class Cons;
class Procedure;

class Expr {
 public:
  Expr(){};
  explicit Expr(Atom atom) : m_kind(Expr_kind::atom), m_atom(atom){};
  explicit Expr(Cons* cons) : m_kind(Expr_kind::cons), m_cons(cons){};
  explicit Expr(token token_) : Expr(Atom(token_)){};
  explicit Expr(Expr_kind kind, Atom atom) : m_kind(kind), m_atom(atom){};
  explicit Expr(Expr_kind kind, Cons* cons) : m_kind(kind), m_cons(cons){};
  explicit Expr(Procedure* proc) : m_kind(Expr_kind::procedure), m_proc(proc){};
  explicit Expr(Expr_kind kind) : m_kind(kind){};

  Expr_kind kind() const { return m_kind; }
  Atom atom() const { return m_atom; }
  Cons* cons() const { return m_cons; }
  Procedure* proc() const { return m_proc; }

  static Expr nil() { return Expr(Expr_kind::nil); }
  static Expr err() { return Expr(Expr_kind::err); }

 private:
  Expr_kind m_kind;
  Atom m_atom;
  Cons* m_cons;
  Procedure* m_proc;
};

class Cons {
 public:
  Cons() = delete;

  static Cons* cons(Expr head, Expr rest) {
    Cons* cons = (Cons*)malloc(sizeof(Cons));
    cons->car = head;
    cons->cdr = rest;
    return cons;
  }

  Expr car;
  Expr cdr;
};

struct symbol_value {
  Atom symbol;
  Expr value;
};

enum class procedure_kind {
  native,
  lambda,
  named_lambda,
};

typedef Expr (*NativeFn)(std::vector<Expr>);
typedef std::vector<symbol_value> Env;

class Procedure {
 public:
  Procedure(Expr params, Expr body, Env env)
      : m_kind(procedure_kind::lambda),
        m_params(params),
        m_body(body),
        m_env(env){};
  Procedure(Atom symbol, Expr params, Expr body, Env env)
      : m_symbol(symbol),
        m_kind(procedure_kind::named_lambda),
        m_params(params),
        m_body(body),
        m_env(env){};
  Procedure(Atom symbol, NativeFn native_fn)
      : m_symbol(symbol),
        m_kind(procedure_kind::native),
        m_native_fn(native_fn){};

  static Procedure* proc(Expr params, Expr body, Env env) {
    Procedure* proc = (Procedure*)malloc(sizeof(Procedure));
    *proc = Procedure(params, body, env);
    return proc;
  }

  static Procedure* proc(Atom symbol, Expr params, Expr body, Env env) {
    Procedure* proc = (Procedure*)malloc(sizeof(Procedure));
    *proc = Procedure(symbol, params, body, env);
    return proc;
  }

  static Procedure* proc(Atom symbol, NativeFn native_fn) {
    Procedure* proc = (Procedure*)malloc(sizeof(Procedure));
    *proc = Procedure(symbol, native_fn);
    return proc;
  }

  Atom symbol() const { return m_symbol; }
  procedure_kind kind() const { return m_kind; }
  Expr params() const { return m_params; }
  Expr body() const { return m_body; }
  NativeFn native_fn() const { return m_native_fn; }
  Env& env() { return m_env; }

 private:
  Atom m_symbol;
  procedure_kind m_kind;
  Expr m_params;
  Expr m_body;
  NativeFn m_native_fn;
  Env m_env;
};
