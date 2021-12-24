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
    return this->make_list2(quote_exp, quoted);
  }
  case token_t::quasiquote: {  // TODO: is there cases like ', ',@ etc?
    this->parse_single_token(Expr_kind::quasiquote);
    Expr quasiquoted = this->parse_head();
    // TODO: check for errors
    if (quasiquoted.kind() == Expr_kind::err) return quasiquoted;
    Expr expanded = this->qq_expand(quasiquoted);
    return expanded;
  }
  case token_t::unquote: {
    Expr unquote_exp = this->parse_single_token(Expr_kind::unquote);
    Expr unquoted = this->parse_head();
    // TODO: check for errors
    if (unquoted.kind() == Expr_kind::err) return unquoted;
    return this->make_list2(unquote_exp, unquoted);
  }
  case token_t::unquote_splicing: {
    Expr unquote_splicing_exp =
        this->parse_single_token(Expr_kind::unquote_splicing);
    Expr unquote_splicing = this->parse_head();
    // TODO: check for errors
    if (unquote_splicing.kind() == Expr_kind::err) return unquote_splicing;
    return this->make_list2(unquote_splicing_exp, unquote_splicing);
  }
  // NOTE: I think these can be treated as symbols and handled seperately inside
  // eval function
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
  case token_t::car:
    return this->parse_single_token(Expr_kind::car);
  case token_t::cdr:
    return this->parse_single_token(Expr_kind::cdr);
  case token_t::cons:
    return this->parse_single_token(Expr_kind::cons_);
  case token_t::eof:
    return Expr::nil();
  default:
    return Expr::err();
  }
}

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

Expr parser::make_list2(Expr e1, Expr e2) {
  return Cons::expr(e1, Cons::expr(e2, Expr::nil()));
}

Expr parser::make_list3(Expr e1, Expr e2, Expr e3) {
  return Cons::expr(e1, Cons::expr(e2, Cons::expr(e3, Expr::nil())));
}

Expr parser::append(Expr front, Expr back) {
  return this->make_list3(Expr(Expr_kind::append), front, back);
}

Expr parser::qq_expand_list(Expr list) {
  switch (list.kind()) {
  case Expr_kind::cons:
    if (CAR(list).kind() == unquote) {
      return this->make_list2(Expr(Expr_kind::list), CADR(list));
    } else if (CAR(list).kind() == unquote_splicing) {
      return CADR(list);
    } else {
      Expr expand_car = qq_expand_list(CAR(list));
      Expr expand_cdr = qq_expand(CDR(list));
      return this->make_list2(Expr(Expr_kind::list),
                              append(expand_car, expand_cdr));
    }
    break;
  default: {
    Expr a_list = Cons::expr(list, Expr::nil());
    return this->make_list2(Expr(Expr_kind::quote), a_list);
  }
  }
}

Expr parser::qq_expand(Expr quoted) {
  switch (quoted.kind()) {
  case Expr_kind::cons:
    if (CAR(quoted).kind() == Expr_kind::unquote) {
      return CADR(quoted);
    } else if (CAR(quoted).kind() == Expr_kind::unquote_splicing) {
      ESQUEMA_ERROR("Ilegal unquote-splicing after quasiquote");
    } else {
      Expr expand_car = this->qq_expand_list(CAR(quoted));
      Expr expand_cdr = this->qq_expand(CDR(quoted));
      return this->append(expand_car, expand_cdr);
    }
  default:
    return this->make_list2(Expr(Expr_kind::quote), quoted);
  }
}
