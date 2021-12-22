#include "esquema/lex.h"
#include "esquema/string_view.h"
#include "esquema/token.h"

lexer::lexer(string_view input)
    : m_input(input.data()), m_original_input(input) {
  m_last_token.loc.end = m_input;
  this->parse_current_token();
}

void lexer::parse_current_token() {
  this->skip_whitespace();
  m_last_token.loc.begin = m_input;
  switch (m_input[0]) {
    CASE_SYMBOL_SPECIAL_CHARS
    m_last_token = this->parse_special_symbols();
    break;
    CASE_NUMBERS
    m_last_token = this->parse_number();
    break;
  case '#':
    m_last_token = this->parse_character();
    break;
  case '(':
    m_last_token = this->parse_single_char_token(token_t::left_paren);
    break;
  case ')':
    m_last_token = this->parse_single_char_token(token_t::right_paren);
    break;
  case '\'':
    m_last_token = this->parse_single_char_token(token_t::quote_abbrev);
    break;
  case '\"':
    m_last_token = this->parse_string_literal();
    break;
  case '\0':
    if (this->is_eof(m_input)) {
      m_last_token.type = token_t::eof;
      m_last_token.loc.end = m_input;
    }
    break;
  default:
    m_last_token = this->parse_symbol();
    break;
  }
}

token lexer::parse_character() {
  const char *begin = m_input;
  const char *it = m_input + 1;

  switch (it[0]) {
  case '\\':
    ++it;
    m_input = it + 1;
    return token(token_t::character, begin, it + 1);
  // TODO: implement
  // https://groups.csail.mit.edu/mac/ftpdir/scheme-7.4/doc-html/scheme_6.html
  case 't':
    ++it;
    m_input = it;
    return token(token_t::true_, begin, it);
  case 'f':
    ++it;
    m_input = it;
    return token(token_t::false_, begin, it);
  default:
    return token(token_t::err, begin, it);
  }
}

token lexer::parse_string_literal() {
  const char *begin = m_input;
  char open_quote = m_input[0];

  const char *it = m_input + 1;

  for (;;) {
    switch (it[0]) {
    case '\0':
      if (this->is_eof(it))
        return token(token_t::err, begin, it);
      else {
        ++it;
        break;
      }
    case '\\': {
      ++it;
      switch (it[0]) {
      case '\0':
        if (this->is_eof(it)) {
          return token(token_t::err, begin, it);
        } else {
          ++it;
          break;
        }
      case '\r':
        ++it;
        if (it[0] == '\n') ++it;
        break;
      // TODO: parse hex and unicode
      default:
        ++it;
        break;
      }
      break;
    }

    case '\"':
    case '\'':
      if (it[0] == open_quote) {
        m_input = ++it;
        return token(token_t::string, begin, it);
      }
      ++it;
      break;
    default:
      ++it;
      break;
    }
  }
}

token lexer::parse_single_char_token(token_t type) {
  const char *begin = m_input;
  m_input += 1;
  return token(type, begin, begin + 1);
}

token lexer::parse_special_symbols() {
  const char *it = m_input;
  switch (it[0]) {
  case '+':
  case '-': {
    char next_char = *(it + 1);
    if (std::isdigit(next_char)) return parse_number();
  }
  default:
    break;
  }
  return parse_symbol();
}

token lexer::parse_symbol() {
  const char *begin = m_input;
  const char *it = m_input;
  for (;;) {
    switch (it[0]) {
      CASE_ALPHABET
      CASE_NUMBERS
      CASE_SYMBOL_SPECIAL_CHARS
      it++;
      break;
    default:
      goto end;
    }
  }
end:
  m_input = it;
  token t = token(token_t::symbol, begin, it);

  if (t.as_string() == "if"_sv) {
    t.type = token_t::if_;
  } else if (t.as_string() == "begin"_sv) {
    t.type = token_t::begin;
  } else if (t.as_string() == "set!"_sv) {
    t.type = token_t::set;
  } else if (t.as_string() == "lambda"_sv) {
    t.type = token_t::lambda;
  } else if (t.as_string() == "function"_sv) {
    t.type = token_t::function;
  } else if (t.as_string() == "named-lambda"_sv) {
    t.type = token_t::named_lambda;
  } else if (t.as_string() == "define"_sv) {
    t.type = token_t::define;
  } else if (t.as_string() == "quote"_sv) {
    t.type = token_t::quote;
  } else if (t.as_string() == "let"_sv) {
    t.type = token_t::let;
  } else if (t.as_string() == "let*"_sv) {
    t.type = token_t::let_star;
  }

  return t;
}

token lexer::parse_number() {
  const char *begin = m_input;
  const char *number_it = m_input;

  while (number_it[0] == '+' || number_it[0] == '-') number_it++;

  while (std::isdigit(number_it[0])) number_it++;

  if (number_it[0] != '.') {
    m_input = number_it;
    return token(token_t::integer, begin, number_it);
  }

  number_it += 1;

  while (std::isdigit(number_it[0])) number_it++;

  m_input = number_it;
  return token(token_t::float_, begin, number_it);
}

void lexer::skip_whitespace() {
  const char *it = m_input;
  while (it[0] == ' ' || it[0] == '\t' || it[0] == '\f' || it[0] == '\v' ||
         it[0] == '\n' || it[0] == '\r') {
    it += 1;
  }
  m_input = it;
}

bool lexer::is_eof(const char *it) {
  return it == this->m_original_input.end();
}
