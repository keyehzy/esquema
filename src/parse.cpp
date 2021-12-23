#include "esquema/expr.h"
#include "esquema/parse.h"
#include "esquema/token.h"

parser::parser(string_view input) : m_lexer(lexer(input)){};

Expr parser::parse_program() {
  if (this->peek().type == token_t::eof) return Expr::nil();
  Expr head = parse_head();
  if (head.kind() == Expr_kind::err) return head;
  Expr rest = parse_program();
  return Expr(new Cons(head, rest));
}

Expr parser::parse_head() {
  switch (this->peek().type) {
  case token_t::integer:
  case token_t::symbol:
  case token_t::float_:
  case token_t::string:
  case token_t::character:
    return this->parse_atom();
  case token_t::left_paren:
    this->skip();
    return this->parse_subexpr();
  case token_t::quote:
    return this->parse_single_token(Expr_kind::quote);
  case token_t::quote_abbrev: {
    Expr quote_exp = this->parse_single_token(Expr_kind::quote_abbrev);
    Expr quoted = this->parse_head();
    // TODO: check for errors
    if (quoted.kind() == Expr_kind::err) return quoted;
    return Expr(new Cons(quote_exp, Expr(new Cons(quoted, Expr::nil()))));
  }
  case token_t::quasiquote: {  // TODO: is there cases like ', ',@ etc?
    Expr quasiquote_exp = this->parse_single_token(Expr_kind::quasiquote);
    Expr quasiquoted = this->parse_head();
    // TODO: check for errors
    if (quasiquoted.kind() == Expr_kind::err) return quasiquoted;
    // Expr expand = this->qq_expand(quasiquote_exp, quasiquoted);
    return Expr(
        new Cons(quasiquote_exp, Expr(new Cons(quasiquoted, Expr::nil()))));
  }
  case token_t::unquote: {
    Expr quote_exp = this->parse_single_token(Expr_kind::unquote);
    Expr quoted = this->parse_head();
    // TODO: check for errors
    if (quoted.kind() == Expr_kind::err) return quoted;
    return Expr(new Cons(quote_exp, Expr(new Cons(quoted, Expr::nil()))));
  }
  case token_t::let:
    return this->parse_single_token(Expr_kind::let);
  case token_t::let_star:
    return this->parse_single_token(Expr_kind::let_star);
  case token_t::letrec:
    return this->parse_single_token(Expr_kind::letrec);
  case token_t::letrec_star:
    return this->parse_single_token(Expr_kind::letrec_star);
  case token_t::if_:
    return this->parse_single_token(Expr_kind::if_);
  case token_t::lambda:
    return this->parse_single_token(Expr_kind::lambda);
  case token_t::begin:
    return this->parse_single_token(Expr_kind::begin);
  case token_t::set:
    return this->parse_single_token(Expr_kind::set);
  case token_t::true_:
    return this->parse_single_token(Expr_kind::true_);
  case token_t::false_:
    return this->parse_single_token(Expr_kind::false_);
  case token_t::function:
    return this->parse_single_token(Expr_kind::function);
  case token_t::named_lambda:
    return this->parse_single_token(Expr_kind::named_lambda);
  case token_t::define:
    return this->parse_single_token(Expr_kind::define);
  case token_t::list:
    return this->parse_single_token(Expr_kind::list);
  case token_t::append:
    return this->parse_single_token(Expr_kind::append);
  case token_t::eof:
    return Expr::nil();
  default:
    return Expr::err();
  }
}

Expr parser::append(Expr front, Expr back) {
  return Expr(new Cons(front, Expr(new Cons(back, Expr::nil()))));
}

// TODO: we need to implement two function before doing this:
// list -> a function a builds a list
// append -> add a item to the end of a list
// Expr parser::qq_expand_list(Expr quote, Expr quoted) {
//   switch (quoted.kind()) {
//   case Expr_kind::cons:
//     if (CAR(quoted).kind() == unquote) {
//       return Expr(
//           new Cons(CADR(quoted), Expr::nil()));  // NOTE: maybe one more cons
//     } else {                                     // TODO: commaat
//       Expr expand_car = qq_expand_list(quote, CAR(quoted));
//       Expr expand_cdr = qq_expand(quote, CDR(quoted));
//       return Cons(CADR
//     }
//   }
// }

// Expr parser::qq_expand(Expr quote, Expr quoted) {
//   switch (quoted.kind()) {
//   case Expr_kind::cons:
//     if (CAR(quoted).kind() == Expr_kind::unquote) {
//       return CADR(quoted);
//     } else {
//       Expr expand_car = this->qq_expand_list(quote, CAR(quoted));
//       Expr expand_cdr = this->qq_expand(quote, CDR(quoted));
//       return append(
//     }
//   default:
//     return Expr(new Cons(quote, Expr(new Cons(quoted, Expr::nil()))));
//   }
// }

Expr parser::parse_subexpr() {
  switch (this->peek().type) {
  case token_t::right_paren:
    this->skip();
    return Expr::nil();
  case token_t::eof:
    return Expr::err();
    // TODO: commaat
  default: {
    Expr head = this->parse_head();
    if (head.kind() == Expr_kind::err) return head;
    Expr rest = this->parse_subexpr();
    return Expr(new Cons(head, rest));
  }
  }
}

Expr parser::parse_atom() {
  Expr exp = Expr(Atom(this->peek()));
  this->skip();
  return exp;
}

Expr parser::parse_single_token(Expr_kind kind) {
  Expr e = Expr(kind, Atom(this->peek()));
  this->skip();
  return e;
}
