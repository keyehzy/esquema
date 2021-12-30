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
  case token_t::left_paren:
    this->skip();
    return this->parse_subexpr();
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
  case token_t::eof:
    return Expr::nil();

  case token_t::is_eq:
  case token_t::is_boolean:
  case token_t::is_null:
  case token_t::is_pair:
  case token_t::is_procedure:
  case token_t::is_symbol:
  case token_t::is_number:
  case token_t::is_string:
  case token_t::is_char:
  case token_t::car:
  case token_t::cdr:
  case token_t::cons:
  case token_t::begin:
  case token_t::define:
  case token_t::false_:
  case token_t::function:
  case token_t::if_:
  case token_t::lambda:
  case token_t::named_lambda:
  case token_t::list:
  case token_t::append:
  case token_t::let:
  case token_t::let_star:
  case token_t::letrec:
  case token_t::letrec_star:
  case token_t::quote:
  case token_t::right_paren:
  case token_t::set:
  case token_t::true_:
  case token_t::integer:
  case token_t::symbol:
  case token_t::float_:
  case token_t::string:
  case token_t::character:
    return this->parse_atom();

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
  Expr exp = Expr(this->peek());
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
  return this->make_list3(Expr(Atom(token_t::append, "append"_sv)), front,
                          back);
}

Expr parser::qq_expand_list(Expr list) {
  switch (list.kind()) {
  case Expr_kind::cons:
    if (CAR(list).kind() == unquote) {
      return this->make_list2(Expr(Atom(token_t::list, "list"_sv)), CADR(list));
    } else if (CAR(list).kind() == unquote_splicing) {
      return CADR(list);
    } else {
      Expr expand_car = qq_expand_list(CAR(list));
      Expr expand_cdr = qq_expand(CDR(list));
      return this->make_list2(Expr(Atom(token_t::list, "list"_sv)),
                              append(expand_car, expand_cdr));
    }
    break;
  default: {
    Expr a_list = Cons::expr(list, Expr::nil());
    return this->make_list2(Expr(Atom(token_t::quote, "'"_sv)), a_list);
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
    return this->make_list2(Expr(Atom(token_t::quote, "'"_sv)), quoted);
  }
}
