#pragma once

#include "string_view.h"

enum class token_t {
  err,
  eof,
  integer,
  float_,
  symbol,
  string,
  character,

  car,
  cdr,
  cons,

  is_boolean,
  is_char,        // TODO
  is_null,        // TODO
  is_pair,        // TODO
  is_procedure,   // TODO
  is_symbol,      // TODO
  is_bytevector,  // TODO
  is_eof_object,  // TODO
  is_number,      // TODO
  is_port,        // TODO
  is_string,      // TODO
  is_vector,      // TODO

  begin,
  define,
  false_,
  function,
  if_,
  lambda,
  named_lambda,
  list,
  append,
  left_paren,
  let,
  let_star,
  letrec,
  letrec_star,
  quote,
  quote_abbrev,
  quasiquote,
  unquote,
  unquote_splicing,
  right_paren,
  set,
  true_,
};

struct location {
  location(){};
  location(const char* begin, const char* end) : begin(begin), end(end){};
  location(string_view sv) : begin(sv.begin()), end(sv.end()){};
  const char* begin;
  const char* end;
  string_view as_string() const { return string_view{begin, end}; }
};

class token {
 public:
  token(){};
  explicit token(token_t type) : type(type){};
  explicit token(token_t type, location loc) : type(type), loc(loc){};
  explicit token(token_t type, string_view sv) : type(type), loc(sv){};
  explicit token(token_t type, const char* begin, const char* end)
      : type(type), loc(location{begin, end}){};

  string_view as_string() const { return loc.as_string(); }
  token_t type;
  location loc;
};
