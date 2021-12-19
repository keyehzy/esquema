#include "esquema/assert.h"
#include <cstdio>

void report_failure(const char* filename, int line, const char* fn_name,
                    const char* msg) {
  std::fprintf(stderr, "%s:%d: assertion failed in %s: %s\n", filename, line,
               fn_name, msg);
}
