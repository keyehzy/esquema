#include "esquema/eval.h"
#include "esquema/expr.h"
#include "esquema/string_view.h"
#include "esquema/token.h"
#include "esquema/native.h"

evaluator::evaluator(string_view input) : m_parser(parser(input)) {
  m_original_value = m_parser.parse_program();
  this->populate_env();
  m_value = this->eval(m_original_value);
};

Expr evaluator::value() const {
  return m_value;
}

Expr evaluator::eval(Expr exp) {
  switch(exp.kind()) {
    case Expr_kind::atom:
      return this->eval_atom(exp);
    case Expr_kind::cons:
      if(CDR(exp).kind() == Expr_kind::nil) {
        if(CAR(exp).kind() != Expr_kind::quote)
          return eval(CAR(exp));
      } else {
        switch(CAR(exp).kind()) {
          case Expr_kind::quote:
            return CADR(exp);
          case Expr_kind::if_:
            if(eval(CADR(exp)).kind() != Expr_kind::false_) {
              return eval(CADDR(exp));
            } else {
              return eval(CADDDR(exp));
            }
          case Expr_kind::begin:
            return this->eprogn(CDR(exp));
          case Expr_kind::set:
            return this->update(CADR(exp), eval(CADDR(exp)));
          default:
            return this->invoke(eval(CAR(exp)), this->flatten(CDR(exp)));
        }
      }
    default:
      return exp;
  }
}

std::vector<Expr> evaluator::flatten(Expr exp) {
  Expr head = exp;
  std::vector<Expr> list;
  while(head.kind() == Expr_kind::cons) {
    list.push_back(eval(CAR(head)));
    head = CDR(head);
  }
  return list;
}

Expr evaluator::invoke(Expr func_exp, std::vector<Expr> args) {
  if(func_exp.kind() == Expr_kind::procedure) {
    return func_exp.m_func(args);
  }
  return Expr::err();
}

Expr evaluator::update(Expr symbol, Expr new_val) {
  for(auto& [key, val] : m_env) {
    if(symbol.atom() == key) {
      val = new_val;
      return new_val;
    }
  }
  m_env.push_back(symbol_value{symbol.atom(), new_val});
  return new_val;
}

Expr evaluator::eprogn(Expr exp) {
  if(exp.kind() == Expr_kind::cons) {
    if(CDR(exp).kind() == Expr_kind::cons) {
      this->eval(CAR(exp));
      return this->eprogn(CDR(exp));
    }
    return this->eval(CAR(exp));
  }
  return Expr::nil();
}

Expr evaluator::eval_atom(Expr exp) {
  switch(exp.atom().token_().type) {
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
  for(const auto& [key, val] : m_env) {
    if(symbol.atom() == key)
      return val;
  }
  return symbol;
}

void evaluator::extend_env(Atom symbol, Expr value) {
  m_env.emplace_back(symbol, value);
}

void evaluator::populate_env() {
  this->extend_env(Atom(token_t::symbol, "+"_sv), Expr(NAT_plus));
  this->extend_env(Atom(token_t::symbol, "-"_sv), Expr(NAT_minus));
  this->extend_env(Atom(token_t::symbol, "*"_sv), Expr(NAT_times));
  this->extend_env(Atom(token_t::symbol, "/"_sv), Expr(NAT_div));
  this->extend_env(Atom(token_t::symbol, "%"_sv), Expr(NAT_mod));
}
