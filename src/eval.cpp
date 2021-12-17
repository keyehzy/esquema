#include "esquema/eval.h"
#include "esquema/expr.h"
#include "esquema/native.h"
#include "esquema/string_view.h"
#include "esquema/token.h"

evaluator::evaluator(string_view input) : m_parser(parser(input)) {
  m_original_value = m_parser.parse_program();
  this->populate_env();
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
        return Expr(Procedure::proc(/*params=*/CADR(exp), /*body=*/CDDR(exp),
                                    m_extended_env));
      case Expr_kind::named_lambda:
        return Expr(Procedure::proc(CAADR(exp).atom(), CDADR(exp), CDDR(exp),
                                    m_extended_env));
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
    Expr baked_named_lambda =
        Expr(Procedure::proc(variable.atom(), params, body, m_extended_env));
    Expr try_set = this->set(variable.atom(), baked_named_lambda);
    if (try_set.kind() != Expr_kind::err)  // set! to bounded variable
      return try_set;
    return this->bind_variable_in_current_env(variable.atom(),
                                              baked_named_lambda);
  } else {
    Expr variable = CADR(exp), expression = eval(CDDR(exp));
    Expr try_set = this->set(variable.atom(), expression);
    if (try_set.kind() != Expr_kind::err)  // set! to bounded variable
      return try_set;
    return this->bind_variable_in_current_env(variable.atom(), expression);
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
    Env copy_env = m_extended_env;
    m_extended_env = this->join(copy_env, fn_exp.proc()->env());
    // TODO: bound checking
    // TODO: flatten proc()->params() so that we get rid of this non-sense
    Expr head = fn_exp.proc()->params();
    for (int index = 0; head.kind() == Expr_kind::cons; index++) {
      this->bind_variable_in_current_env(CAR(head).atom(), args[index]);
      head = CDR(head);
    }
    Expr result = this->eprogn(fn_exp.proc()->body());
    m_extended_env = copy_env;
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
  switch (exp.atom().token_().type) {
  case token_t::integer:
  case token_t::string:
  case token_t::float_:
    return exp;
  case token_t::symbol:
    return this->lookup_symbol(exp);
  default:
    return Expr::err();
  }
}

Expr evaluator::set(Atom symbol, Expr new_val) {
  for (auto& [key, val] : m_extended_env) {
    if (symbol == key) {
      val = new_val;
      return val;
    }
  }
  return Expr::err();  // set! on an unbound variable
}

Expr evaluator::lookup_symbol(Expr symbol) {
  for (auto& [key, val] : m_extended_env) {
    if (symbol.atom() == key) return val;
  }
  for (auto& [key, val] : m_initial_env) {
    if (symbol.atom() == key) return val;
  }
  return symbol;  // autoquote
}

void evaluator::populate_env() {
  auto set_native_fn = [&](Atom symbol, NativeFn fn) {
    m_initial_env.emplace_back(
        symbol, Expr(Procedure::proc(symbol, fn, m_extended_env)));
  };
  set_native_fn(Atom("+"_sv), NAT_plus);
  set_native_fn(Atom("-"_sv), NAT_minus);
  set_native_fn(Atom("*"_sv), NAT_times);
  set_native_fn(Atom("/"_sv), NAT_div);
  set_native_fn(Atom("%"_sv), NAT_mod);
}

// TODO: We need to differenciete between nothing and nil
Expr evaluator::bind_variable_in_current_env(Atom symbol, Expr value) {
  m_extended_env.emplace_back(symbol, value);
  return value;
}

Env evaluator::join(const Env& dst, const Env& src) {
  Env new_env = dst;
  for (const auto& sv : src) new_env.push_back(sv);
  return new_env;
}
