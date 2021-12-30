#pragma once

#include <cassert>
#include <esquema/string_view.h>
#include <variant>
#include <vector>

#define OPTIONS_UNIMPLEMENTED()                           \
  do {                                                    \
    this->crash_gracefully(__FILE__, __LINE__, __func__); \
  } while (false)

#define OPTIONS_CASE_LOWERCASE_LETTERS \
  case 'a':                            \
  case 'b':                            \
  case 'c':                            \
  case 'd':                            \
  case 'e':                            \
  case 'f':                            \
  case 'g':                            \
  case 'h':                            \
  case 'i':                            \
  case 'j':                            \
  case 'k':                            \
  case 'l':                            \
  case 'm':                            \
  case 'n':                            \
  case 'o':                            \
  case 'p':                            \
  case 'q':                            \
  case 'r':                            \
  case 's':                            \
  case 't':                            \
  case 'u':                            \
  case 'v':                            \
  case 'w':                            \
  case 'x':                            \
  case 'y':                            \
  case 'z'

#define OPTIONS_CASE_UPPERCASE_LETTERS \
  case 'A':                            \
  case 'B':                            \
  case 'C':                            \
  case 'D':                            \
  case 'E':                            \
  case 'F':                            \
  case 'G':                            \
  case 'H':                            \
  case 'I':                            \
  case 'J':                            \
  case 'K':                            \
  case 'L':                            \
  case 'M':                            \
  case 'N':                            \
  case 'O':                            \
  case 'P':                            \
  case 'Q':                            \
  case 'R':                            \
  case 'S':                            \
  case 'T':                            \
  case 'U':                            \
  case 'V':                            \
  case 'W':                            \
  case 'X':                            \
  case 'Y':                            \
  case 'Z'

#define OPTIONS_CASE_NUMBERS \
  case '0':                  \
  case '1':                  \
  case '2':                  \
  case '3':                  \
  case '4':                  \
  case '5':                  \
  case '6':                  \
  case '7':                  \
  case '8':                  \
  case '9'

enum class option_type {
  _invalid,
  file,
  eval,
  repl,
  compile,
};

struct option {
  option_type type;
  string_view arg;
};

class argparse {
 public:
  explicit argparse(int argc, char** argv);

 private:
  string_view peek();
  void skip();
  void skip_whitespace();
  void expect_token_then_skip(char);
  string_view parse_long_name();

  option parse_current_option();
  option parse_file_path();

 public:
  void parse_options();
  std::vector<option> options;

  string_view program_name() const noexcept { return this->program_name_; }

  int size() const noexcept { return this->size_; }

  string_view input(int idx) const noexcept {
    assert(idx >= 0 && idx < static_cast<int>(this->inputs_.size()));
    return this->inputs_[idx];
  }

 private:
  void crash_gracefully(const char*, int, const char*);

  string_view program_name_;
  int size_;
  int index_;
  const char* position_;
  std::vector<string_view> inputs_;
};

bool operator==(option o1, option o2);
bool operator!=(option o1, option o2);
