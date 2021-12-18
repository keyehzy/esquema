#pragma once

#include "expr.h"
#include <vector>

Expr NAT_plus(std::vector<Expr> exps);
Expr NAT_minus(std::vector<Expr> exps);
Expr NAT_times(std::vector<Expr> exps);
Expr NAT_div(std::vector<Expr> exps);
Expr NAT_mod(std::vector<Expr> exps);
Expr NAT_eq(std::vector<Expr> exps);
Expr NAT_gt(std::vector<Expr> exps);
Expr NAT_ge(std::vector<Expr> exps);
Expr NAT_lt(std::vector<Expr> exps);
Expr NAT_le(std::vector<Expr> exps);
