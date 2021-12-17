#include "esquema/eval.h"
#include "esquema/expr.h"
#include "esquema/lex.h"
#include "esquema/parse.h"
#include "esquema/print.h"
#include "esquema/string_view.h"
#include "esquema/token.h"
#include <iostream>

void pprint(Expr exp, string& str) {
  // TODO: this function should use some sort of formatted-string
  switch (exp.kind()) {
  case Expr_kind::atom:
    if (exp.atom().is_evaluated) {
      if (exp.atom().type() == token_t::integer) {
        str.append(std::to_string(exp.atom().as_int()));
      } else if (exp.atom().type() == token_t::float_) {
        str.append(std::to_string(exp.atom().as_float()));
      } else {
        str.append("ERROR!!!");
      }
    } else {
      str.append(exp.atom().as_string());
    }
    break;

  case Expr_kind::cons:
    if (CDR(exp).kind() == Expr_kind::nil) {
      pprint(CAR(exp), str);
    } else if (CADR(exp).kind() == Expr_kind::atom) {
      str.append("(");
      Expr head = exp;
      while (head.kind() == Expr_kind::cons) {
        pprint(CAR(head), str);
        if (CDR(head).kind() == Expr_kind::cons) str.append(" ");
        head = CDR(head);
      }
      str.append(")");
    } else {
      str.append("(");
      pprint(CAR(exp), str);
      str.append(" ");
      pprint(CDR(exp), str);
      str.append(")");
    }
    break;

  case Expr_kind::procedure:
    if (exp.proc()->kind() == procedure_kind::lambda) {
      str.append("(closure (t)");
      if (exp.proc()->params().kind() == Expr_kind::nil) {
        str.append(" () ");
      } else if (CDR(exp.proc()->params()).kind() == Expr_kind::nil) {
        str.append(" (");
        pprint(exp.proc()->params(), str);
        str.append(") ");
      } else {
        str.append(" ");
        pprint(exp.proc()->params(), str);
        str.append(" ");
      }
      pprint(exp.proc()->body(), str);
      str.append(")");
    } else if (exp.proc()->kind() == procedure_kind::named_lambda) {
      str.append("(closure ");
      str.append(exp.proc()->symbol().as_string());
      str.append(" (t)");
      if (CDR(exp.proc()->params()).kind() == Expr_kind::nil) {
        str.append(" (");
        pprint(exp.proc()->params(), str);
        str.append(") ");
      } else {
        str.append(" ");
        pprint(exp.proc()->params(), str);
        str.append(" ");
      }
      pprint(exp.proc()->body(), str);
      str.append(")");
    } else if (exp.proc()->kind() == procedure_kind::native) {
      str.append("(native");
      str.append(" \"");
      str.append(exp.proc()->symbol().as_string());
      str.append("\" ");
    }
    break;

  case Expr_kind::lambda:
    str.append("lambda");
    break;

  case Expr_kind::if_:
    str.append("if");
    break;

  case Expr_kind::nil:
    str.append("()");
    break;

  case Expr_kind::err:
    str.append("ERROR!!!");
    break;
  }
}

string print1(Expr exp) {
  string res = "";
  pprint(exp, res);
  return res;
}

string print_quoted(string_view s) { return print1(parser(s).parse_program()); }

string print(string_view s) { return print1(evaluator(s).value()); }
