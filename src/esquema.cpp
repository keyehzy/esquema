#include "esquema/print.h"
#include "esquema/string_view.h"
#include <editline/readline.h>
#include <iostream>

int main(void) {
  char *buffer;
  while ((buffer = readline("esquema> ")) != NULL) {
    string res = print(buffer);
    if (!res.empty()) {
      std::cout << res << std::endl;
    }
  }
  return 0;
}
