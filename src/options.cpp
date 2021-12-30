#include <cctype>
#include <cstdio>
#include <esquema/assert.h>
#include <esquema/options.h>
#include <esquema/string_view.h>

argparse::argparse(int argc, char** argv) : size_(argc), index_(0) {
  assert(this->size_ >= 0);
  this->program_name_ = argv[this->index_];
  this->position_ = program_name_.end();
  for (int i = 1; i < this->size_; i++) {
    this->inputs_.emplace_back(argv[i]);
  }
}

string_view argparse::peek() { return this->inputs_[this->index_]; }

void argparse::skip() {
  assert(this->index_ + 1 < this->size_);  // TODO(keyehzh): report error
  this->index_ += 1;
  this->position_ = this->peek().begin();
  if (this->position_ != nullptr) this->skip_whitespace();
}

void argparse::skip_whitespace() {
  const char* input = this->position_;
  while (input[0] == ' ' || input[0] == '\t' || input[0] == '\f' ||
         input[0] == '\v' || input[0] == '\n' || input[0] == '\r') {
    input += 1;
  }
  this->position_ = input;
}

void argparse::expect_token_then_skip(char token) {
  if (this->position_[0] != token) {
    OPTIONS_UNIMPLEMENTED();  // TODO(keyehzh): report error
  }
  this->position_ += 1;
}

option argparse::parse_current_option() {
  this->skip_whitespace();
  this->expect_token_then_skip('-');
  switch (this->position_[0]) {
  case '-': {  // parse as long
    expect_token_then_skip('-');
    string_view longname = this->parse_long_name();
    if (longname == "file") {
      this->skip();
      option filepath = this->parse_file_path();
      this->index_ += 1;
      return filepath;
    }

    if (longname == "compile") {
      this->skip();
      option o{.type = option_type::compile, .arg = ""_sv};
      this->index_ += 1;
      return o;
    }
    OPTIONS_UNIMPLEMENTED();
    break;
  }

  default:  // parse as short
    switch (this->position_[0]) {
    case 'f': {
      this->skip();
      option filepath = this->parse_file_path();
      this->index_ += 1;
      return filepath;
    }

    case 'c': {
      this->skip();
      option o{.type = option_type::compile, .arg = ""_sv};
      this->index_ += 1;
      return o;
    }

    default:
      OPTIONS_UNIMPLEMENTED();  // TODO(keyehzh): report error no option
      break;
    }
    break;
  }
  ESQUEMA_NOT_REACHED();  // TODO(keyehzh): report error invalid
}

option argparse::parse_file_path() {
  const char* input = this->position_;
  while (true) {
    switch (input[0]) {
    OPTIONS_CASE_LOWERCASE_LETTERS:
    OPTIONS_CASE_UPPERCASE_LETTERS:
    OPTIONS_CASE_NUMBERS:
    case '/':
    case '~':
    case '.':
    case '-':
      input += 1;
      continue;

    case '\0':
      goto end;

    default:
      this->position_ = input;
      return option{.type = option_type::_invalid, .arg = this->peek()};
    }
  }
end:
  this->position_ = input;
  return option{.type = option_type::file, .arg = this->peek()};
}

string_view argparse::parse_long_name() {
  const char* input = this->position_;
  while (std::isalpha(input[0])) {
    input += 1;
  }
  string_view name(this->position_, input);
  this->position_ = input;
  return name;
}

void argparse::parse_options() {
  if (this->size_ == 1) {
    this->options.emplace_back(option_type::repl, ""_sv);
  }

  while (this->index_ + 1 < this->size_) {
    this->position_ = this->peek().begin();
    option o = this->parse_current_option();

    if (o.type == option_type::_invalid)
      OPTIONS_UNIMPLEMENTED();  // TODO report error

    this->options.push_back(o);
  }
}

void argparse::crash_gracefully(const char* filename, int line,
                                const char* funcname) {
  std::fprintf(stderr,
               "%s:%d option `%s` not implemented and crashed in `%s`\n",
               filename, line, this->peek().data(), funcname);
  __builtin_trap();
}

bool operator==(option o1, option o2) {
  return o1.type == o2.type && o1.arg == o2.arg;
}

bool operator!=(option o1, option o2) {
  return o1.type != o2.type || o1.arg != o2.arg;
}
