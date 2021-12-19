#pragma once

#include "expr.h"
#include <vector>

Expr NAT_plus(List exps);
Expr NAT_minus(List exps);
Expr NAT_times(List exps);
Expr NAT_div(List exps);
Expr NAT_mod(List exps);
Expr NAT_eq(List exps);
Expr NAT_gt(List exps);
Expr NAT_ge(List exps);
Expr NAT_lt(List exps);
Expr NAT_le(List exps);
