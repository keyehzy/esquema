#include "esquema/assert.h"
#include "esquema/options.h"
#include "esquema/print.h"
#include "esquema/string_view.h"
#include <editline/readline.h>
#include <fstream>
#include <iostream>

int main(int argc, char** argv) {
  argparse args(argc, argv);
  std::string buffer;
  args.parse_options();
  for (const auto& o : args.options) {
    switch (o.type) {
    case option_type::file: {
      string_view filepath = o.arg;
      std::ifstream file(filepath.data(), std::ios::binary | std::ios::ate);
      std::streamsize size = file.tellg();
      file.seekg(0, std::ios::beg);
      buffer.resize(size);
      if (file.read(buffer.data(), size)) {
        printer p(buffer);
        std::cout << p.print() << std::endl;
      }
      break;
    }

    case option_type::eval: {
      string_view input = o.arg;
      printer p(input);
      std::cout << p.print() << std::endl;
      break;
    }

    case option_type::repl: {
      char* buffer;
      while ((buffer = readline("esquema> ")) != NULL) {
        printer p(buffer);
        std::cout << p.print() << std::endl;
      }
      break;
    }

    default:
      ESQUEMA_NOT_REACHED();  // TODO(keyehzh): usage/help page
      break;
    }
  }
  return 0;
}
