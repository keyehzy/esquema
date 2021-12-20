#pragma once

#include "assert.h"
#include "heap.h"
#include "token.h"
#include <vector>

static Heap heap(5 * MiB);

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
  character,
  define,
  function,
  false_,
  if_,
  lambda,
  let,
  named_lambda,
  procedure,
  quote,
  quote_abbrev,
  set,
  true_,
};

class Cons;
class Procedure;
struct symbol_value;
typedef std::vector<symbol_value> Env;

class Expr {
 public:
  Expr(){};
  explicit Expr(Atom atom) : m_kind(Expr_kind::atom), m_atom(atom){};
  explicit Expr(Cons* cons) : m_kind(Expr_kind::cons), m_cons(cons){};
  explicit Expr(token token_) : Expr(Atom(token_)){};
  explicit Expr(int i) : Expr(Atom(i)){};
  explicit Expr(double f) : Expr(Atom(f)){};
  explicit Expr(bool cond)
      : m_kind(cond ? Expr_kind::true_ : Expr_kind::false_){};
  explicit Expr(Expr_kind kind, Atom atom) : m_kind(kind), m_atom(atom){};
  explicit Expr(Expr_kind kind, Cons* cons) : m_kind(kind), m_cons(cons){};
  explicit Expr(Procedure* proc, Env env)
      : m_kind(Expr_kind::procedure), m_proc(proc), m_env(env){};
  explicit Expr(Expr_kind kind) : m_kind(kind){};

  Expr_kind kind() const { return m_kind; }
  Atom atom() const { return m_atom; }
  Cons* cons() const { return m_cons; }
  Procedure* proc() const { return m_proc; }
  Env& env() { return m_env; }

  static Expr nil() { return Expr(Expr_kind::nil); }
  static Expr err() { return Expr(Expr_kind::err); }

 private:
  Expr_kind m_kind;
  Atom m_atom;
  Cons* m_cons;
  Procedure* m_proc;
  Env m_env;
};

struct symbol_value {
  Atom symbol;
  Expr value;
};

class Cons {
 public:
  Cons() = delete;
  Cons(Expr head, Expr rest) : car(head), cdr(rest){};

  static void* operator new(size_t size) {
    void* block = heap.allocate(size);
    ESQUEMA_ASSERT(block);
    return block;
  }

  static void operator delete(void*) {}

  Expr car;
  Expr cdr;
};

enum class procedure_kind {
  native,
  lambda,
  named_lambda,
};

// TODO: We want a general purpose linked list instead of stl containers
typedef std::vector<Expr> List;
typedef Expr (*NativeFn)(List);

class Procedure {
 public:
  Procedure(Expr params, Expr body)
      : m_kind(procedure_kind::lambda), m_params(params), m_body(body){};
  Procedure(Atom symbol, Expr params, Expr body)
      : m_symbol(symbol),
        m_kind(procedure_kind::named_lambda),
        m_params(params),
        m_body(body){};
  Procedure(Atom symbol, NativeFn native_fn)
      : m_symbol(symbol),
        m_kind(procedure_kind::native),
        m_native_fn(native_fn){};

  static void* operator new(size_t size) {
    void* block = heap.allocate(size);
    ESQUEMA_ASSERT(block);
    return block;
  }

  static void operator delete(void*) {}

  Atom symbol() const { return m_symbol; }
  procedure_kind kind() const { return m_kind; }
  Expr params() const { return m_params; }
  Expr body() const { return m_body; }
  NativeFn native_fn() const { return m_native_fn; }

 private:
  Atom m_symbol;
  procedure_kind m_kind;
  Expr m_params;
  Expr m_body;
  NativeFn m_native_fn;
};
