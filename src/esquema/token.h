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

  begin,
  define,
  false_,
  function,
  if_,
  lambda,
  named_lambda,
  left_paren,
  quote,
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
  string_view as_string() { return string_view{begin, end}; }
};

class token {
 public:
  token(){};
  explicit token(token_t type) : type(type){};
  explicit token(token_t type, location loc) : type(type), loc(loc){};
  explicit token(token_t type, string_view sv) : type(type), loc(sv){};
  explicit token(token_t type, const char* begin, const char* end)
      : type(type), loc(location{begin, end}){};

  string_view as_string() { return loc.as_string(); }
  token_t type;
  location loc;
};
