#include "esquema/eval.h"
#include "esquema/expr.h"
#include "esquema/native.h"
#include "esquema/string_view.h"
#include "esquema/token.h"

evaluator::evaluator(string_view input) : m_parser(parser(input)) {
  m_original_value = m_parser.parse_program();
  this->populate_env();
  m_value = this->eval(m_original_value, m_toplevel_env);
};

Expr evaluator::value() const { return m_value; }

Expr evaluator::eval(Expr exp, Env& env) {
  switch (exp.kind()) {
  case Expr_kind::atom:
    return this->eval_atom(exp, env);
  case Expr_kind::cons:
    if (CDR(exp).kind() == Expr_kind::nil &&
        CAR(exp).kind() != Expr_kind::list) {
      return eval(CAR(exp), env);
    } else {
      switch (CAR(exp).kind()) {
      case Expr_kind::car:
        ESQUEMA_ASSERT(CDR(exp).kind() == Expr_kind::cons);
        return CAR(this->eval(CDR(exp), env));
      case Expr_kind::cdr:
        ESQUEMA_ASSERT(CDR(exp).kind() == Expr_kind::cons);
        return CDR(this->eval(CDR(exp), env));
      case Expr_kind::cons_:
        ESQUEMA_ASSERT(CDR(exp).kind() == Expr_kind::cons);
        // TODO: the code (cons 'a 'b) is invalid because 'b is not a list
        return Cons::expr(this->eval(CADR(exp), env),
                          this->eval(CDDR(exp), env));
      case Expr_kind::quote:
      case Expr_kind::quote_abbrev:
        return CADR(exp);
      case Expr_kind::if_:
        if (eval(CADR(exp), env).kind() != Expr_kind::false_) {
          return eval(CADDR(exp), env);
        } else {
          return eval(CADDDR(exp), env);
        }
      case Expr_kind::lambda:
        return Expr(
            new Procedure(/*params=*/CADR(exp), /*body=*/CDDR(exp), env));
      case Expr_kind::named_lambda:
        return Expr(new Procedure(/*symbol=*/CAADR(exp).atom(),
                                  /*params=*/CDADR(exp), /*body=*/CDDR(exp),
                                  env));
      case Expr_kind::list:
        if (CDR(exp).kind() == Expr_kind::nil) return CDR(exp);
        ESQUEMA_ASSERT(CDR(exp).kind() == Expr_kind::cons);
        return this->build_list(CDR(exp), env);

      case Expr_kind::append:
        ESQUEMA_ASSERT(CDDR(exp).kind() == Expr_kind::cons);
        return this->append_list(this->eval(CADR(exp), env), CDDR(exp), env);

      case Expr_kind::let:
      case Expr_kind::let_star:
        return this->eval_let(exp, env);

      // NOTE: The difference between these two, as fair as I understand, is
      // just in the order of evaluation, maybe allowing some optimizations.
      case Expr_kind::letrec:
      case Expr_kind::letrec_star:
        return this->eval_letrec(exp, env);

      case Expr_kind::define:
        return this->eval_define(exp, env);
      case Expr_kind::begin:
        return this->eprogn(CDR(exp), env);
      case Expr_kind::set:
        return this->set(CADR(exp).atom(), eval(CADDR(exp), env), env);
      default:
        return this->invoke(eval(CAR(exp), env),
                            this->eval_list(CDR(exp), env));
      }
    }
    break;
  case Expr_kind::procedure:
  case Expr_kind::true_:
  case Expr_kind::false_:
  case Expr_kind::nil:
    return exp;
  default:
    ESQUEMA_ERROR("Could not evaluate expression");
    return Expr::err();
  }
  ESQUEMA_NOT_REACHED();
}

Expr evaluator::eval_letrec(Expr exp, Env& env) {
  bool was_already_inside_lambda = is_inside_lambda;
  is_inside_lambda = true;

  Expr variable_inits = CADR(exp);
  Expr body = CDDR(exp);
  Env extended_env;
  extended_env.extend_from(env);

  // 1) The variables are bound to fresh locations holding unassigned
  // values, the inits are evaluated in the extended environment
  Expr it = variable_inits;
  while (it.kind() != Expr_kind::nil) {
    Expr variable = CAAR(it);
    this->bind_variable(variable, Expr::nil(), extended_env);
    it = CDR(it);
  }

  // 2) each variable is assigned to the result of the corresponding
  // init
  it = variable_inits;
  while (it.kind() != Expr_kind::nil) {
    Expr variable = CAAR(it);
    Expr init = CADAR(it);
    Expr value = this->eval(init, extended_env);
    this->set(variable.atom(), value, extended_env);
    it = CDR(it);
  }

  // 3) the exprs are evaluated sequentially in the extended
  // environment, and the value of the last expr is returned
  Expr result = this->eprogn(body, extended_env);
  is_inside_lambda = was_already_inside_lambda;
  return result;
}

Expr evaluator::eval_let(Expr exp, Env& env) {
  bool was_already_inside_lambda = is_inside_lambda;
  is_inside_lambda = true;

  Expr variable_inits = CADR(exp);
  Expr body = CDDR(exp);
  Env let_env;
  let_env.extend_from(env);
  Expr it = variable_inits;
  while (it.kind() != Expr_kind::nil) {
    Expr variable = CAAR(it);
    Expr init = CADAR(it);
    Expr value = CAR(exp).kind() == Expr_kind::let ? this->eval(init, env)
                                                   : this->eval(init, let_env);
    this->bind_variable(variable, value, let_env);
    it = CDR(it);
  }
  Expr result = this->eprogn(body, let_env);
  is_inside_lambda = was_already_inside_lambda;
  return result;
}

/*
(define append
(lambda (ls1 ls2)
  (if (null? ls1)
    ls2
    (cons (car ls1) (append (cdr ls1) ls2)))))
*/
Expr evaluator::append_list(Expr list1, Expr list2, Env& env) {
  if (list1.kind() == Expr_kind::nil) {
    return this->eval(list2, env);
  }
  return Cons::expr(CAR(list1), this->append_list(CDR(list1), list2, env));
}

Expr evaluator::build_list(Expr exp, Env& env) {
  if (exp.kind() == Expr_kind::nil) {
    return exp;
  }
  return Cons::expr(this->eval(CAR(exp), env), this->build_list(CDR(exp), env));
}

// See sec. 2.4
// https://www.gnu.org/software/mit-scheme/documentation/stable/mit-scheme-ref.pdf
Expr evaluator::eval_define(Expr exp, Env& env) {
  ESQUEMA_ASSERT(is_at_top_level(env) || is_inside_lambda);
  if (CADR(exp).kind() == Expr_kind::cons) {
    Expr variable = CAADR(exp), params = CDADR(exp), body = CDDR(exp);
    Env temp_env;
    Expr baked_named_lambda =
        Expr(new Procedure(/*symbol=*/variable.atom(), /*params=*/params,
                           /*body=*/body, temp_env));
    Expr try_set = this->set(variable.atom(), baked_named_lambda, env);
    if (try_set.kind() != Expr_kind::err)  // set! to bounded variable
      return try_set;
    return this->bind_variable(variable, baked_named_lambda, env);
  } else {
    Expr variable = CADR(exp), expression = eval(CDDR(exp), env);
    Expr try_set = this->set(variable.atom(), expression, env);
    if (try_set.kind() != Expr_kind::err)  // set! to bounded variable
      return try_set;
    return this->bind_variable(variable, expression, env);
  }
}

List evaluator::eval_list(Expr exp, Env& env) {
  Expr head = exp;
  List list;
  while (head.kind() == Expr_kind::cons) {
    Expr value = eval(CAR(head), env);
    list.push_back(value);
    head = CDR(head);
  }
  return list;
}

Expr evaluator::invoke(Expr fn_exp, List args) {
  if (fn_exp.kind() != Expr_kind::procedure) {
    ESQUEMA_ERROR("An expression of kind 'procedure' was expected.");
    return Expr::err();
  }
  switch (fn_exp.proc()->kind()) {
  case procedure_kind::native:
    return fn_exp.proc()->native_fn()(args);
  case procedure_kind::named_lambda:
  case procedure_kind::lambda: {
    bool was_already_inside_lambda = is_inside_lambda;
    is_inside_lambda = true;

    Expr head = fn_exp.proc()->params();

    Env invocation_env;
    invocation_env.extend_from(fn_exp.proc()->closing_env());

    // TODO: bound checking
    for (int index = 0; head.kind() == Expr_kind::cons; index++) {
      this->bind_variable(CAR(head), args[index], invocation_env);
      head = CDR(head);
    }
    Expr result = this->eprogn(fn_exp.proc()->body(), invocation_env);
    is_inside_lambda = was_already_inside_lambda;
    return result;
  }
  }
  ESQUEMA_NOT_REACHED();
}

Expr evaluator::eprogn(Expr exp, Env& env) {
  Expr head = exp;
  Expr val = head;
  while (head.kind() == Expr_kind::cons) {
    val = this->eval(CAR(head), env);

    // TODO: Check for errors
    if (val.kind() == Expr_kind::err) return val;

    head = CDR(head);
  }
  return val;
}

Expr evaluator::eval_atom(Expr exp, Env& env) {
  switch (exp.atom().token_().type) {
  case token_t::character:  // TODO: maybe we can evaluate these here?
  case token_t::float_:
  case token_t::integer:
  case token_t::string:
    return exp;
  case token_t::symbol:
    return this->lookup_symbol(exp, env);
  default:
    ESQUEMA_ERROR("Could not evaluate atom");
    return Expr::err();
  }
}

Expr evaluator::set(Atom symbol, Expr new_val, Env& env) {
  // Section (2.5) MIT-Scheme Reference:

  // 1) If expression is specified, evaluates expression and stores the
  // resulting value in the location to which variable is bound. If expression
  // is omitted, variable is altered to be unassigned; a subsequent reference to
  // such a variable is an error. In either case, the value of the set!
  // expression is unspecified.

  EnvNode* node = env.find_last(symbol);
  if (node != nullptr) {
    node->value = new_val;
    return new_val;
  }

  // 2) Variable must be bound either in some region enclosing the set!
  // expression, or at the top level. However, variable is permitted to be
  // unassigned when the set! form is entered.
  node = m_toplevel_env.find_last(symbol);
  if (node != nullptr) {
    node->value = new_val;
    return new_val;
  }

  return Expr::err();  // set! on an unbound variable
}

Expr evaluator::lookup_symbol(Expr symbol, Env env) {
  EnvNode* node = m_protected_env.find_last(symbol.atom());
  if (node != nullptr) {
    return node->value;
  }

  node = env.find_last(symbol.atom());
  if (node != nullptr) {
    if (node->value.kind() == Expr_kind::nil) {
      return symbol;
    }
    return node->value;
  }

  node = m_toplevel_env.find_last(symbol.atom());
  if (node != nullptr) {
    if (node->value.kind() == Expr_kind::nil) {
      return symbol;
    }
    return node->value;
  }

  return symbol;  // autoquote
}

void evaluator::populate_env() {
  auto set_native_fn = [&](Atom symbol, NativeFn fn) {
    m_protected_env.add(symbol, Expr(new Procedure(symbol, fn)));
  };
  set_native_fn(Atom("+"_sv), NAT_plus);
  set_native_fn(Atom("-"_sv), NAT_minus);
  set_native_fn(Atom("*"_sv), NAT_times);
  set_native_fn(Atom("/"_sv), NAT_div);
  set_native_fn(Atom("%"_sv), NAT_mod);
  set_native_fn(Atom("="_sv), NAT_eq);
  set_native_fn(Atom(">"_sv), NAT_gt);
  set_native_fn(Atom(">="_sv), NAT_ge);
  set_native_fn(Atom("<"_sv), NAT_lt);
  set_native_fn(Atom("<="_sv), NAT_le);
}

// TODO: We need to differentiate between nothing and nil
Expr evaluator::bind_variable(Expr symbol, Expr value, Env& env) {
  env.add(symbol.atom(), value);

  if (value.kind() == Expr_kind::nil) {
    return symbol;
  }

  return value;
}

bool evaluator::is_at_top_level(const Env& env) {
  return &env == &m_toplevel_env;  // TODO: better checking
}
