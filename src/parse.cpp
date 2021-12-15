#include "esquema/parse.h"
#include "esquema/token.h"

parser::parser(string_view input) : m_lexer(lexer(input)) {};

Expr parser::parse_program() {
  if(this->peek().type == token_t::eof)
    return Expr::nil();
  Expr head = parse_head();
  Expr rest = parse_program();
  return Expr(Cons::cons(head, rest));
}

Expr parser::parse_head() {
  switch(this->peek().type) {
    case token_t::integer:
    case token_t::symbol:
    case token_t::float_:
    case token_t::string:
      return this->parse_atom();
    case token_t::left_paren:
      this->skip();
      return this->parse_subexpr();
    case token_t::quote: {
      Expr quote = Expr(Expr_kind::quote, Atom(this->peek()));
      this->skip();
      return quote;
    }
    case token_t::if_: {
      Expr if_exp = Expr(Expr_kind::if_, Atom(this->peek()));
      this->skip();
      return if_exp;
    }
    case token_t::begin: {
      Expr begin_exp = Expr(Expr_kind::begin, Atom(this->peek()));
      this->skip();
      return begin_exp;
    }
    case token_t::set: {
      Expr set_exp = Expr(Expr_kind::set, Atom(this->peek()));
      this->skip();
      return set_exp;
    }
    case token_t::true_: {
      Expr true_exp = Expr(Expr_kind::true_, Atom(this->peek()));
      this->skip();
      return true_exp;
    }
    case token_t::false_: {
      Expr false_exp = Expr(Expr_kind::false_, Atom(this->peek()));
      this->skip();
      return false_exp;
    }
    case token_t::eof:
      this->skip();
      return Expr::nil();
    default:
      this->skip();
      return Expr::err();
  }
}

Expr parser::parse_subexpr() {
  switch(this->peek().type) {
    case token_t::right_paren:
      this->skip();
      return Expr::nil();
    default: {
      Expr head = this->parse_head();
      Expr rest = this->parse_subexpr();
      return Expr(Cons::cons(head, rest));
    }
  }
}

Expr parser::parse_atom() {
  Expr exp = Expr(Atom(this->peek()));
  this->skip();
  return exp;
}
