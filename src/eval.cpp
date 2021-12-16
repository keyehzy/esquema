#include "esquema/eval.h"
#include "esquema/expr.h"
#include "esquema/native.h"
#include "esquema/string_view.h"
#include "esquema/token.h"

evaluator::evaluator(string_view input) : m_parser(parser(input)) {
  m_original_value = m_parser.parse_program();
  this->populate_env();
  this->push_scope(&m_env);
  m_value = this->eval(m_original_value);
};

Expr evaluator::value() const { return m_value; }

Expr evaluator::eval(Expr exp) {
  switch (exp.kind()) {
  case Expr_kind::atom:
    return this->eval_atom(exp);
  case Expr_kind::cons:
    if (CDR(exp).kind() == Expr_kind::nil) {
      if (CAR(exp).kind() != Expr_kind::quote) return eval(CAR(exp));
    } else {
      switch (CAR(exp).kind()) {
      case Expr_kind::quote:
        return CADR(exp);
      case Expr_kind::if_:
        if (eval(CADR(exp)).kind() != Expr_kind::false_) {
          return eval(CADDR(exp));
        } else {
          return eval(CADDDR(exp));
        }
      case Expr_kind::lambda:
        return Expr(Procedure::proc(CADR(exp), CDDR(exp)));
      case Expr_kind::named_lambda:
        return Expr(Procedure::proc(CAADR(exp).atom(), CDADR(exp), CDDR(exp)));
      case Expr_kind::begin:
        return this->eprogn(CDR(exp));
      case Expr_kind::set:
        return this->update(CADR(exp), eval(CADDR(exp)));
      default:
        return this->invoke(eval(CAR(exp)), this->eval_list(CDR(exp)));
      }
    }
  default:
    return exp;
  }
}

std::vector<Expr> evaluator::eval_list(Expr exp) {
  Expr head = exp;
  std::vector<Expr> list;
  while (head.kind() == Expr_kind::cons) {
    list.push_back(eval(CAR(head)));
    head = CDR(head);
  }
  return list;
}

Expr evaluator::invoke(Expr fn_exp, std::vector<Expr> args) {
  if (fn_exp.kind() != Expr_kind::procedure) return Expr::err();
  switch (fn_exp.proc()->kind()) {
  case procedure_kind::native:
    return fn_exp.proc()->native_fn()(args);
  case procedure_kind::named_lambda:
  case procedure_kind::lambda: {
    this->push_scope(&fn_exp.proc()->env());
    // TODO: bound checking
    // TODO: flatten proc()->params() so that we get rid of this non-sense
    Expr head = fn_exp.proc()->params();
    for (int index = 0; head.kind() == Expr_kind::cons; index++) {
      this->push_to_current_scope(CAR(head).atom(), args[index]);
      head = CDR(head);
    }
    Expr result = this->eprogn(fn_exp.proc()->body());
    this->pop_scope();
    return result;
  }
  }
}

Expr evaluator::update(Expr symbol, Expr new_val) {
  // Search from first search from recent to older scope
  for (auto it = m_scopes.rbegin(); it != m_scopes.rend(); it++) {
    Env* local_env = *it;
    for (auto& [key, val] : *local_env) {
      if (symbol.atom() == key) {
        val = new_val;
        return new_val;
      }
    }
  }

  // If we don't find it, declare it in the global scope
  this->push_to_global_scope(symbol.atom(), new_val);
  return new_val;
}

Expr evaluator::eprogn(Expr exp) {
  Expr head = exp;
  Expr val = head;
  while (head.kind() == Expr_kind::cons) {
    val = this->eval(CAR(head));

    // TODO: Check for errors
    if (val.kind() == Expr_kind::err) return val;

    head = CDR(head);
  }
  return eval(val);
}

Expr evaluator::eval_atom(Expr exp) {
  switch (exp.atom().token_().type) {
  case token_t::integer:
  case token_t::string:
  case token_t::float_:
    return exp;
  case token_t::symbol:
    return this->eval_symbol(exp);
  default:
    return Expr::err();
  }
}

Expr evaluator::eval_symbol(Expr symbol) {
  // Search from most recent scope to most older one
  for (auto it = m_scopes.rbegin(); it != m_scopes.rend(); it++) {
    Env* local_env = *it;
    for (auto& [key, val] : *local_env) {
      if (symbol.atom() == key) return val;
    }
  }
  return symbol;
}

void evaluator::set_native_fn(Atom symbol, NativeFn fn) {
  m_env.emplace_back(symbol, Expr(Procedure::proc(symbol, fn)));
}

void evaluator::populate_env() {
  this->set_native_fn(Atom("+"_sv), NAT_plus);
  this->set_native_fn(Atom("-"_sv), NAT_minus);
  this->set_native_fn(Atom("*"_sv), NAT_times);
  this->set_native_fn(Atom("/"_sv), NAT_div);
  this->set_native_fn(Atom("%"_sv), NAT_mod);
}

void evaluator::push_scope(Env* scope) { m_scopes.push_back(scope); }

void evaluator::pop_scope() { m_scopes.pop_back(); }

Env* evaluator::get_current_scope() { return m_scopes.back(); }

void evaluator::push_to_current_scope(Atom symbol, Expr value) {
  this->get_current_scope()->emplace_back(symbol, value);
}

void evaluator::push_to_global_scope(Atom symbol, Expr value) {
  m_env.emplace_back(symbol, value);
}
