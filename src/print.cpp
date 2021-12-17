#include "esquema/eval.h"
#include "esquema/expr.h"
#include "esquema/lex.h"
#include "esquema/padded_string.h"
#include "esquema/parse.h"
#include "esquema/print.h"
#include "esquema/string_view.h"
#include "esquema/token.h"
#include <iostream>

#define PAREN_BLOCK(block) \
  do {                     \
    this->append("(");     \
    block;                 \
    this->append(")");     \
  } while (0)

#define RPAD_PAREN_BLOCK(block) \
  do {                          \
    this->append("(");          \
    block;                      \
    this->append(") ");         \
  } while (0)

void printer::pprint(Expr exp, padding pad = padding::none) {
  // TODO: this function should use some sort of formatted-string
  switch (exp.kind()) {
  case Expr_kind::atom:
    this->print_atom(exp, pad);
    break;
  // TODO: Redo this following the same flow as eval function
  case Expr_kind::cons:
    this->print_cons(exp);
    break;
  // TODO: closures are always returning (t), which is wrong. See
  // https://nullprogram.com/blog/2013/12/30/ for a example of clusure
  // returning contents from its environment case Expr_kind::procedure:
  //   if (exp.proc()->kind() == procedure_kind::lambda) {
  //     this->append("(closure (t)"_sv);
  //     if (exp.proc()->params().kind() == Expr_kind::nil) {
  //       this->append(" nil "_sv);
  //     } else if (CDR(exp.proc()->params()).kind() == Expr_kind::nil) {
  //       this->append(" ("_sv);
  //       pprint(exp.proc()->params());
  //       this->append(") "_sv);
  //     } else {
  //       this->append(" "_sv);
  //       pprint(exp.proc()->params());
  //       this->append(" "_sv);
  //     }
  //     pprint(exp.proc()->body());
  //     this->append(")"_sv);
  //   } else if (exp.proc()->kind() == procedure_kind::named_lambda) {
  //     this->append("(closure "_sv);
  //     this->append(exp.proc()->symbol().as_string());
  //     this->append(" (t)"_sv);
  //     if (CDR(exp.proc()->params()).kind() == Expr_kind::nil) {
  //       this->append(" ("_sv);
  //       pprint(exp.proc()->params());
  //       this->append(") "_sv);
  //     } else {
  //       this->append(" "_sv);
  //       pprint(exp.proc()->params());
  //       this->append(" "_sv);
  //     }
  //     pprint(exp.proc()->body());
  //     this->append(")"_sv);
  //   } else if (exp.proc()->kind() == procedure_kind::native) {
  //     this->append("(native"_sv);
  //     this->append(" \""_sv);
  //     this->append(exp.proc()->symbol().as_string());
  //     this->append("\" "_sv);
  //   }
  //   break;

  // case Expr_kind::lambda:
  //   this->append("lambda"_sv);
  //   if (CADR(exp).kind() == Expr_kind::nil) {
  //     this->append(" nil "_sv);
  //   } else if (CDR(CADR(exp)).kind() == Expr_kind::nil) {
  //     this->append(" ("_sv);
  //     pprint(CADR(exp));
  //     this->append(") "_sv);
  //   } else {
  //     this->append(" ("_sv);
  //     pprint(CADR(exp));
  //     this->append(") "_sv);
  //   }
  //   pprint(CDDR(exp));
  //   this->append(")"_sv);
  //   break;

  // case Expr_kind::if_:
  //   this->append("if"_sv);
  //   break;

  // case Expr_kind::nil:
  //   this->append("()"_sv);
  //   break;

  // case Expr_kind::err:
  //   this->append("ERROR!!!"_sv);
  //   break;
  // }
  case Expr_kind::nil:
    this->append("()"_sv, pad);
    break;

  default:
    this->append("ERROR!!!"_sv);
    break;
  }
}

void printer::print_cons(Expr exp) {
  if (CDR(exp).kind() == Expr_kind::nil) {
    if (CAR(exp).kind() != Expr_kind::quote) return this->pprint(CAR(exp));
  } else {
    switch (CAR(exp).kind()) {
    case Expr_kind::quote:
      PAREN_BLOCK(this->append("quote"_sv, padding::right);
                  this->pprint(CADR(exp)););
      break;
    case Expr_kind::if_:
      PAREN_BLOCK(this->append("if"_sv, padding::right);
                  this->pprint(CADR(exp)); this->pprint(CADDR(exp));
                  this->pprint(CADDDR(exp)););
      break;
    case Expr_kind::lambda:
      PAREN_BLOCK(this->append("lambda"_sv, padding::right);
                  RPAD_PAREN_BLOCK(this->pprint(CADR(exp)););
                  this->pprint(CDDR(exp)););  // FIXME: nested paren_block
      break;
    case Expr_kind::named_lambda:
      PAREN_BLOCK(this->append("named-lambda"_sv, padding::right);
                  this->append(CAADR(exp).atom().as_string());
                  this->pprint(CDADR(exp)); this->pprint(CDDR(exp)););
      break;
    case Expr_kind::define:
      PAREN_BLOCK(this->append("define"_sv, padding::right);
                  this->append(CAADR(exp).atom().as_string());
                  this->pprint(CDADR(exp)); this->pprint(CDDR(exp)););
      break;
    case Expr_kind::begin: {
      PAREN_BLOCK(this->append("begin"_sv, padding::right);
                  this->print_block(exp););
      break;
    }
    case Expr_kind::set:
      PAREN_BLOCK(this->append("set!"_sv, padding::right);
                  this->append(CADR(exp).atom().as_string());
                  this->pprint(CADDR(exp)););
      break;
    case Expr_kind::procedure:
      this->print_procedure(exp);
      break;
    default:
      PAREN_BLOCK(this->pprint(CAR(exp), padding::right);
                  this->print_block(CDR(exp)););
      break;
    }
  }
}

void printer::print_block(Expr exp) {
  Expr head = exp;
  while (head.kind() != Expr_kind::nil) {
    if (CDR(head).kind() != Expr_kind::nil)
      this->pprint(CAR(head), padding::right);
    else
      this->pprint(CAR(head), padding::none);
    head = CDR(head);
  };
}

void printer::print_procedure(Expr exp) {
  if (exp.kind() != Expr_kind::procedure) this->append("ERROR!!!"_sv);
  switch (exp.proc()->kind()) {  // TODO: lambda and named-lambda closures
  case procedure_kind::native:
    PAREN_BLOCK(this->append(exp.proc()->symbol().as_string());
                this->pprint(exp.proc()->params()););
    break;
  }
}

void printer::print_atom(Expr exp, padding pad = padding::none) {
  if (exp.atom().is_evaluated) {
    this->print_evaluated_atom(exp, pad);
  } else {
    this->append(exp.atom().as_string(), pad);
  }
}

void printer::print_evaluated_atom(Expr exp, padding pad) {
  if (exp.atom().type() == token_t::integer) {
    this->append(std::to_string(exp.atom().as_int()), pad);
  } else if (exp.atom().type() == token_t::float_) {
    this->append(std::to_string(exp.atom().as_float()), pad);
  } else {
    this->append("ERROR!!!"_sv, pad);
  }
}

void printer::append(string_view sv) { m_buffer.emplace_back(sv); }

void printer::append(string_view sv, padding pad) {
  m_buffer.emplace_back(sv, pad);
}

string printer::print1(Expr exp) {
  this->pprint(exp);
  return this->dump_buffer();
}

string printer::dump_buffer() {
  string s;
  for (const padded_string& pad_str : m_buffer) {
    switch (pad_str.padding_kind()) {
    case padding::left:
      s.append(" ");
      s.append(pad_str.to_string_view());
      break;
    case padding::right:
      s.append(pad_str.to_string_view());
      s.append(" ");
      break;
    case padding::both:
      s.append(" ");
      s.append(pad_str.to_string_view());
      s.append(" ");
      break;
    case padding::none:
      s.append(pad_str.to_string_view());
      break;
    }
  }
  return s;
}

string printer::print_quoted() {
  return print1(parser(m_original_input).parse_program());
}

string printer::print() {
  return this->print1(evaluator(m_original_input).value());
}
