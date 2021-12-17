#include "esquema/print.h"
#include "esquema/string_view.h"
#include <editline/readline.h>
#include <iostream>

int main(void) {
  char *buffer;
  while ((buffer = readline("esquema> ")) != NULL) {
    printer p(buffer);
    string res = p.print();
    if (!res.empty()) {
      std::cout << res << std::endl;
    }
  }
  return 0;
}
