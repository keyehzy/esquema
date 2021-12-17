#pragma once

#include "expr.h"
#include "padded_string.h"
#include "string_view.h"

class printer {
 public:
  explicit printer(string_view sv) : m_original_input(sv){};
  explicit printer(const char* str) : m_original_input(string_view(str)){};
  string print_quoted();
  string print();

 private:
  string dump_buffer();
  void append(string_view);
  void append(string_view, padding);
  string print1(Expr);
  void pprint(Expr, padding);
  void print_atom(Expr, padding);
  void print_evaluated_atom(Expr, padding);
  void print_cons(Expr);
  void print_procedure(Expr);
  void print_block(Expr);

  string_view m_original_input;
  std::vector<padded_string> m_buffer;
};
