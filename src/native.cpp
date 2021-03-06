#include "esquema/eval.h"
#include "esquema/native.h"
#include "esquema/string_view.h"
#include <cmath>
#include <functional>

Expr NAT_binary_operation_int(Atom a0, Atom a1,
                              std::function<int(int, int)> operation) {
  int a0_integer, a1_integer;

  if (a0.is_evaluated) {
    a0_integer = a0.as_int();
  } else {
    a0_integer = std::strtol(a0.as_string().data(), NULL, 10);
  }

  if (a1.is_evaluated) {
    a1_integer = a1.as_int();
  } else {
    a1_integer = std::strtol(a1.as_string().data(), NULL, 10);
  }

  int result = operation(a0_integer, a1_integer);
  return Expr(result);
}

Expr NAT_binary_operation_double(
    Atom a0, Atom a1, std::function<double(double, double)> operation) {
  double a0_float, a1_float;

  if (a0.is_evaluated) {
    a0_float = a0.as_float();
  } else {
    a0_float = std::strtod(a0.as_string().data(), NULL);
  }

  if (a1.is_evaluated) {
    a1_float = a1.as_float();
  } else {
    a1_float = std::strtod(a1.as_string().data(), NULL);
  }

  double result = operation(a0_float, a1_float);
  return Expr(result);
}

Expr NAT_plus(List exps) {
  Atom a0 = exps[0].atom(), a1 = exps[1].atom();
  if (a0.type() == token_t::integer && a1.type() == token_t::integer) {
    return NAT_binary_operation_int(a0, a1,
                                    [](int a, int b) -> int { return a + b; });
  } else if (a0.type() == token_t::float_ || a1.type() == token_t::float_) {
    return NAT_binary_operation_double(
        a0, a1, [](double a, double b) -> double { return a + b; });
  } else {
    return Expr::err();
  }
}

Expr NAT_minus(List exps) {
  Atom a0 = exps[0].atom(), a1 = exps[1].atom();
  if (a0.type() == token_t::integer && a1.type() == token_t::integer) {
    return NAT_binary_operation_int(a0, a1,
                                    [](int a, int b) -> int { return a - b; });
  } else if (a0.type() == token_t::float_ || a1.type() == token_t::float_) {
    return NAT_binary_operation_double(
        a0, a1, [](double a, double b) -> double { return a - b; });
  } else {
    return Expr::err();
  }
}

Expr NAT_times(List exps) {
  Atom a0 = exps[0].atom(), a1 = exps[1].atom();
  if (a0.type() == token_t::integer && a1.type() == token_t::integer) {
    return NAT_binary_operation_int(a0, a1,
                                    [](int a, int b) -> int { return a * b; });
  } else if (a0.type() == token_t::float_ || a1.type() == token_t::float_) {
    return NAT_binary_operation_double(
        a0, a1, [](double a, double b) -> double { return a * b; });
  } else {
    return Expr::err();
  }
}

Expr NAT_div(List exps) {
  Atom a0 = exps[0].atom(), a1 = exps[1].atom();
  if (a0.type() == token_t::integer && a1.type() == token_t::integer) {
    return NAT_binary_operation_int(a0, a1,
                                    [](int a, int b) -> int { return a / b; });
  } else if (a0.type() == token_t::float_ || a1.type() == token_t::float_) {
    return NAT_binary_operation_double(
        a0, a1, [](double a, double b) -> double { return a / b; });
  } else {
    return Expr::err();
  }
}

Expr NAT_mod(List exps) {
  Atom a0 = exps[0].atom(), a1 = exps[1].atom();
  if (a0.type() == token_t::integer && a1.type() == token_t::integer) {
    return NAT_binary_operation_int(a0, a1,
                                    [](int a, int b) -> int { return a % b; });
  } else if (a0.type() == token_t::float_ || a1.type() == token_t::float_) {
    return NAT_binary_operation_double(
        a0, a1, [](double a, double b) -> double { return std::fmod(a, b); });
  } else {
    return Expr::err();
  }
}

Expr NAT_binary_conditional_operation_int(
    Atom a0, Atom a1, std::function<bool(int, int)> operation) {
  int a0_integer, a1_integer;

  if (a0.is_evaluated) {
    a0_integer = a0.as_int();
  } else {
    a0_integer = std::strtol(a0.as_string().data(), NULL, 10);
  }

  if (a1.is_evaluated) {
    a1_integer = a1.as_int();
  } else {
    a1_integer = std::strtol(a1.as_string().data(), NULL, 10);
  }

  bool result = operation(a0_integer, a1_integer);
  return Expr(result);
}

Expr NAT_binary_conditional_operation_double(
    Atom a0, Atom a1, std::function<bool(double, double)> operation) {
  double a0_float, a1_float;

  if (a0.is_evaluated) {
    a0_float = a0.as_float();
  } else {
    a0_float = std::strtod(a0.as_string().data(), NULL);
  }

  if (a1.is_evaluated) {
    a1_float = a1.as_float();
  } else {
    a1_float = std::strtod(a1.as_string().data(), NULL);
  }

  bool result = operation(a0_float, a1_float);
  return Expr(result);
}

Expr NAT_eq(List exps) {
  Atom a0 = exps[0].atom(), a1 = exps[1].atom();
  if (a0.type() == token_t::integer && a1.type() == token_t::integer) {
    return NAT_binary_conditional_operation_int(
        a0, a1, [](int a, int b) -> bool { return a == b; });
  } else if (a0.type() == token_t::float_ || a1.type() == token_t::float_) {
    return NAT_binary_conditional_operation_double(
        a0, a1, [](double a, double b) -> bool { return a == b; });
  } else {
    return Expr::err();
  }
}

Expr NAT_gt(List exps) {
  Atom a0 = exps[0].atom(), a1 = exps[1].atom();
  if (a0.type() == token_t::integer && a1.type() == token_t::integer) {
    return NAT_binary_conditional_operation_int(
        a0, a1, [](int a, int b) -> bool { return a > b; });
  } else if (a0.type() == token_t::float_ || a1.type() == token_t::float_) {
    return NAT_binary_conditional_operation_double(
        a0, a1, [](double a, double b) -> bool { return a > b; });
  } else {
    return Expr::err();
  }
}

Expr NAT_ge(List exps) {
  Atom a0 = exps[0].atom(), a1 = exps[1].atom();
  if (a0.type() == token_t::integer && a1.type() == token_t::integer) {
    return NAT_binary_conditional_operation_int(
        a0, a1, [](int a, int b) -> bool { return a >= b; });
  } else if (a0.type() == token_t::float_ || a1.type() == token_t::float_) {
    return NAT_binary_conditional_operation_double(
        a0, a1, [](double a, double b) -> bool { return a >= b; });
  } else {
    return Expr::err();
  }
}

Expr NAT_lt(List exps) {
  Atom a0 = exps[0].atom(), a1 = exps[1].atom();
  if (a0.type() == token_t::integer && a1.type() == token_t::integer) {
    return NAT_binary_conditional_operation_int(
        a0, a1, [](int a, int b) -> bool { return a < b; });
  } else if (a0.type() == token_t::float_ || a1.type() == token_t::float_) {
    return NAT_binary_conditional_operation_double(
        a0, a1, [](double a, double b) -> bool { return a < b; });
  } else {
    return Expr::err();
  }
}

Expr NAT_le(List exps) {
  Atom a0 = exps[0].atom(), a1 = exps[1].atom();
  if (a0.type() == token_t::integer && a1.type() == token_t::integer) {
    return NAT_binary_conditional_operation_int(
        a0, a1, [](int a, int b) -> bool { return a <= b; });
  } else if (a0.type() == token_t::float_ || a1.type() == token_t::float_) {
    return NAT_binary_conditional_operation_double(
        a0, a1, [](double a, double b) -> bool { return a <= b; });
  } else {
    return Expr::err();
  }
}
