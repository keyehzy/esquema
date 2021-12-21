#pragma once

#include "assert.h"
#include "heap.h"
#include "token.h"
#include <functional>
#include <optional>
#include <vector>

static Heap arena(5 * MiB);

class Atom {
 public:
  Atom() = default;
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
  string_view as_string() const { return m_token.as_string(); }
  int as_int() const { return m_value_int; }
  double as_float() const { return m_value_float; }

  bool operator==(const Atom& a) const {
    return this->type() == a.type() && this->as_string() == a.as_string();
  }

  bool operator!=(const Atom& a) const {
    return this->type() != a.type() || this->as_string() != a.as_string();
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

class Expr {
 public:
  Expr() = default;
  explicit Expr(Atom atom) : m_kind(Expr_kind::atom), m_atom(atom){};
  explicit Expr(Cons* cons) : m_kind(Expr_kind::cons), m_cons(cons){};
  explicit Expr(token token_) : Expr(Atom(token_)){};
  explicit Expr(int i) : Expr(Atom(i)){};
  explicit Expr(double f) : Expr(Atom(f)){};
  explicit Expr(bool cond)
      : m_kind(cond ? Expr_kind::true_ : Expr_kind::false_){};
  explicit Expr(Expr_kind kind, Atom atom) : m_kind(kind), m_atom(atom){};
  explicit Expr(Expr_kind kind, Cons* cons) : m_kind(kind), m_cons(cons){};
  explicit Expr(Procedure* proc) : m_kind(Expr_kind::procedure), m_proc(proc){};
  explicit Expr(Expr_kind kind) : m_kind(kind){};

  Expr_kind kind() const { return m_kind; }
  Atom atom() const { return m_atom; }
  Atom& atom() { return m_atom; }
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
  Cons(Expr head, Expr rest) : car(head), cdr(rest){};

  static void* operator new(size_t size, Heap& heap = arena) {
    return heap.allocate(size);
  }

  static void operator delete(void*) {}

  static Cons* cons(Expr head, Expr rest, Heap& heap) {
    return new (heap) Cons(head, rest);
  }

  Expr car;
  Expr cdr;
};

struct EnvNode {
  explicit EnvNode(Atom symbol, Expr value)
      : symbol(symbol), value(value), next(nullptr){};

  static void* operator new(size_t size, Heap& heap = arena) {
    return heap.allocate(size);
  }

  static void operator delete(void*) {}

  // Used for testing only
  static EnvNode* env(Atom symbol, Expr value, Heap& heap) {
    return new (heap) EnvNode(symbol, value);
  }

  Atom symbol;
  Expr value;
  EnvNode* next;
};

class Env {
 public:
  Env() : m_root(nullptr), m_last(nullptr), m_size(0){};

  void add(Atom symbol, Expr value) {
    EnvNode* node = new EnvNode(symbol, value);

    if (m_root) {
      m_last->next = node;
      m_last = node;
    } else {
      m_root = node;
      m_last = node;
    }

    m_size += 1;
  }

  EnvNode* get(int index) {
    ESQUEMA_ASSERT(index < m_size);
    EnvNode* it = m_root;
    int pos = 0;
    while (it != nullptr && pos < index) {
      it = it->next;
      pos += 1;
    }
    return it;
  }

  EnvNode* find(Atom symbol) {
    EnvNode* it = m_root;
    while (it != nullptr && it->symbol != symbol) {
      it = it->next;
    }
    return it;
  }

  void forEach(const std::function<void(const Atom&, const Expr&)>& operation) {
    EnvNode* it = m_root;
    while (it != nullptr) {
      operation(it->symbol, it->value);
      it = it->next;
    }
  }

 private:
  EnvNode* m_root;
  EnvNode* m_last;
  int m_size;
};

// TODO: We want a general purpose linked list instead of stl containers
typedef std::vector<Expr> List;
typedef Expr (*NativeFn)(List);

enum class procedure_kind {
  native,
  lambda,
  named_lambda,
};

class Procedure {
 public:
  Procedure(Expr params, Expr body, Env env)
      : m_kind(procedure_kind::lambda),
        m_params(params),
        m_body(body),
        m_closure(env){};
  Procedure(Atom symbol, Expr params, Expr body, Env env)
      : m_symbol(symbol),
        m_kind(procedure_kind::named_lambda),
        m_params(params),
        m_body(body),
        m_closure(env){};
  Procedure(Atom symbol, NativeFn native_fn)
      : m_symbol(symbol),
        m_kind(procedure_kind::native),
        m_native_fn(native_fn){};

  static void* operator new(size_t size, Heap& heap = arena) {
    return heap.allocate(size);
  }

  static void operator delete(void*) {}

  // Used for testing only
  static Procedure* proc(Expr params, Expr body, Heap& heap) {
    return new (heap) Procedure(params, body, {});
  }

  // Used for testing only
  static Procedure* proc(Atom symbol, Expr params, Expr body, Heap& heap) {
    return new (heap) Procedure(symbol, params, body, {});
  }

  // Used for testing only
  static Procedure* proc(Atom symbol, NativeFn native_fn, Heap& heap) {
    return new (heap) Procedure(symbol, native_fn);
  }

  Atom symbol() const { return m_symbol; }
  procedure_kind kind() const { return m_kind; }
  Expr params() const { return m_params; }
  Expr body() const { return m_body; }
  NativeFn native_fn() const { return m_native_fn; }
  Env& env() { return m_env; }
  Env& closure() { return m_closure; }

 private:
  Atom m_symbol;
  procedure_kind m_kind;
  Expr m_params;
  Expr m_body;
  NativeFn m_native_fn;
  Env m_env;
  Env m_closure;
};
