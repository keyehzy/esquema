#include "esquema/eval.h"
#include "esquema/expr.h"
#include "esquema/native.h"
#include "esquema/string_view.h"
#include "esquema/token.h"

evaluator::evaluator(string_view input) : m_parser(parser(input)) {
  m_original_value = m_parser.parse_program();
  this->populate_env();
  this->push_scope(&m_initial_env);
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
      case Expr_kind::quote_abbrev:
        return CADR(exp);
      case Expr_kind::if_:
        if (eval(CADR(exp)).kind() != Expr_kind::false_) {
          return eval(CADDR(exp));
        } else {
          return eval(CADDDR(exp));
        }
      case Expr_kind::lambda:
        return Expr(Procedure::proc(/*params=*/CADR(exp), /*body=*/CDDR(exp),
                                    *this->get_scope()));
      case Expr_kind::named_lambda:
        return Expr(Procedure::proc(/*symbol=*/CAADR(exp).atom(),
                                    /*params=*/CDADR(exp), /*body=*/CDDR(exp),
                                    *this->get_scope()));
      case Expr_kind::define:
        return this->eval_define(exp);
      case Expr_kind::begin:
        return this->eprogn(CDR(exp));
      case Expr_kind::set:
        return this->set(CADR(exp).atom(), eval(CADDR(exp)));
      default:
        return this->invoke(eval(CAR(exp)), this->eval_list(CDR(exp)));
      }
    }
  default:
    return exp;
  }
}

// TODO: this the second form of *define*, implement the first one. See sec. 2.4
// https://www.gnu.org/software/mit-scheme/documentation/stable/mit-scheme-ref.pdf
Expr evaluator::eval_define(Expr exp) {
  if (CADR(exp).kind() == Expr_kind::cons) {
    Expr variable = CAADR(exp), params = CDADR(exp), body = CDDR(exp);
    Expr baked_named_lambda = Expr(
        Procedure::proc(variable.atom(), params, body, *this->get_scope()));
    Expr try_set = this->set(variable.atom(), baked_named_lambda);
    if (try_set.kind() != Expr_kind::err)  // set! to bounded variable
      return try_set;
    return this->bind_variable_in_current_env(variable, baked_named_lambda);
  } else {
    Expr variable = CADR(exp), expression = eval(CDDR(exp));
    Expr try_set = this->set(variable.atom(), expression);
    if (try_set.kind() != Expr_kind::err)  // set! to bounded variable
      return try_set;
    return this->bind_variable_in_current_env(variable, expression);
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
    Expr head = fn_exp.proc()->params();  // TODO: flatten proc()->params() so
                                          // that we get rid of this non-sense
    for (int index = 0; head.kind() == Expr_kind::cons; index++) {
      this->bind_variable_in_current_env(CAR(head),
                                         args[index]);  // TODO: bound checking
      head = CDR(head);
    }
    Expr result = this->eprogn(fn_exp.proc()->body());
    this->pop_scope();
    return result;
  }
  }
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
  switch (exp.atom().token_().type) { // TODO: maybe we can evaluate these here?
  case token_t::character:
  case token_t::float_:
  case token_t::integer:
  case token_t::string:
    return exp;
  case token_t::symbol:
    return this->lookup_symbol(exp);
  default:
    return Expr::err();
  }
}

Expr evaluator::set(Atom symbol, Expr new_val) {
  // Section (2.5) MIT-Scheme Reference:
  // If expression is specified, evaluates expression and stores the resulting
  // value in the location to which variable is bound. If expression is omitted,
  // variable is altered to be unassigned; a subsequent reference to such a
  // variable is an error. In either case, the value of the set! expression is
  // unspecified.
  // TODO: unassgined state for variable
  // TODO: unspecified value for unassigned variable

  // Variable must be bound either in some region enclosing the set! expression,
  // or at the top level. However, variable is permitted to be unassigned when
  // the set! form is entered.
  for (Env* env : m_scope_collection) {
    for (auto& [key, val] : *env) {
      if (symbol == key) {
        val = new_val;
        return val;
      }
    }
  }
  return Expr::err();  // set! on an unbound variable
}

Expr evaluator::lookup_symbol(Expr symbol) {
  for (Env* env : m_scope_collection) {
    for (auto& [key, val] : *env) {
      if (symbol.atom() == key) return val;
    }
  }

  for (auto& [key, val] : m_protected_env) {
    if (symbol.atom() == key) return val;
  }

  return symbol;  // autoquote
}

void evaluator::populate_env() {
  auto set_native_fn = [&](Atom symbol, NativeFn fn) {
    m_protected_env.emplace_back(symbol, Expr(Procedure::proc(symbol, fn)));
  };
  set_native_fn(Atom("+"_sv), NAT_plus);
  set_native_fn(Atom("-"_sv), NAT_minus);
  set_native_fn(Atom("*"_sv), NAT_times);
  set_native_fn(Atom("/"_sv), NAT_div);
  set_native_fn(Atom("%"_sv), NAT_mod);
  set_native_fn(Atom(">"_sv), NAT_gt);
  set_native_fn(Atom("<"_sv), NAT_lt);
}

// TODO: We need to differenciete between nothing and nil
Expr evaluator::bind_variable_in_current_env(Expr symbol, Expr value) {
  this->get_scope()->emplace_back(symbol.atom(), value);
  return symbol;
}

void evaluator::extend(Env& dst, const Env& src) {
  for (const auto& sv : src) {
    dst.push_back(sv);
  }
}

Env* evaluator::get_scope() { return m_scope_collection.back(); }

void evaluator::push_scope(Env* env) { m_scope_collection.push_back(env); }

void evaluator::pop_scope() { m_scope_collection.pop_back(); }
