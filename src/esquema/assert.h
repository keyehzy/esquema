#include <cstdlib>

#define ESQUEMA_ASSERT(...)                                       \
  do {                                                            \
    if (__VA_ARGS__) {                                            \
    } else {                                                      \
      report_failure(__FILE__, __LINE__, __func__, #__VA_ARGS__); \
      __builtin_trap();                                           \
    }                                                             \
  } while (false)

void report_failure(const char* filename, int line, const char* function_name,
                    const char* message);
