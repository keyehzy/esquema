#include <iostream>

#include "esquema/string_view.h"
#include "esquema/print.h"
#include "esquema/lex.h"
#include "esquema/parse.h"
#include "esquema/eval.h"
#include "esquema/token.h"

void pprint(Expr exp, string& str) {
  switch(exp.kind()) {
    case Expr_kind::atom:
      if(exp.atom().is_evaluated) {
        if(exp.atom().type() == token_t::integer) {
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
      if(exp.cons()->cdr.kind() == Expr_kind::nil) {
        pprint(exp.cons()->car, str);
      } else {
        str.append("(");
        pprint(exp.cons()->car, str);
        str.append(" ");
        pprint(exp.cons()->cdr, str);
        str.append(")");
      }
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

string print_quoted(string_view s) {
  return print1(parser(s).parse_program());
}

string print(string_view s) {
  return print1(evaluator(s).value());
}
