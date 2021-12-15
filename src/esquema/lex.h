#pragma once

#include "string_view.h"
#include "token.h"

#define ENUMERATE_NUMBERS(O) \
  O('0')                     \
  O('1')                     \
  O('2')                     \
  O('3')                     \
  O('4')                     \
  O('5')                     \
  O('6')                     \
  O('7')                     \
  O('8')                     \
  O('9')

#define ENUMERATE_LOWERCASE_ALPHABET(O) \
  O('a')                                \
  O('b')                                \
  O('c')                                \
  O('d')                                \
  O('e')                                \
  O('f')                                \
  O('g')                                \
  O('h')                                \
  O('i')                                \
  O('j')                                \
  O('k')                                \
  O('l')                                \
  O('m')                                \
  O('n')                                \
  O('o')                                \
  O('p')                                \
  O('q')                                \
  O('r')                                \
  O('s')                                \
  O('t')                                \
  O('u')                                \
  O('v')                                \
  O('x')                                \
  O('w')                                \
  O('y')                                \
  O('z')

#define ENUMERATE_UPPERCASE_ALPHABET(O) \
  O('A')                                \
  O('B')                                \
  O('C')                                \
  O('D')                                \
  O('E')                                \
  O('F')                                \
  O('G')                                \
  O('H')                                \
  O('I')                                \
  O('J')                                \
  O('K')                                \
  O('L')                                \
  O('M')                                \
  O('N')                                \
  O('O')                                \
  O('P')                                \
  O('Q')                                \
  O('R')                                \
  O('S')                                \
  O('T')                                \
  O('U')                                \
  O('V')                                \
  O('X')                                \
  O('W')                                \
  O('Y')                                \
  O('Z')

#define ENUMERATE_SYMBOL_SPECIAL_CHARS(O) \
  O('+')                                  \
  O('-')                                  \
  O('*')                                  \
  O('/')                                  \
  O('%')                                  \
  O('=')                                  \
  O('<')                                  \
  O('>')                                  \
  O('!')

#define CASE(e) case e:
#define CASE_NUMBERS ENUMERATE_NUMBERS(CASE)
#define CASE_CHARS ENUMERATE_NUMBERS(CASE)
#define CASE_ALPHABET                \
  ENUMERATE_LOWERCASE_ALPHABET(CASE) \
  ENUMERATE_UPPERCASE_ALPHABET(CASE)
#define CASE_SYMBOL_SPECIAL_CHARS ENUMERATE_SYMBOL_SPECIAL_CHARS(CASE)

class lexer {
 public:
  explicit lexer(string_view);

  token& peek() { return this->m_last_token; }
  void skip() { this->parse_current_token(); }
  void parse_current_token();
  token parse_number();
  token parse_character();
  token parse_special_symbols();
  token parse_symbol();
  token parse_single_char_token(token_t);
  token parse_string_literal();
  void skip_whitespace();
  bool is_eof(const char*);

 private:
  const char* m_input;
  string_view m_original_input;
  token m_last_token;
};
