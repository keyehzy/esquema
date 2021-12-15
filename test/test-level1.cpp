#include <esquema/print.h>
#include <gtest/gtest.h>

TEST(test, level1) {
  // Testing read of numbers
  { EXPECT_EQ(print_quoted("1"), "1"); }
  { EXPECT_EQ(print_quoted("7"), "7"); }
  { EXPECT_EQ(print_quoted("  7   "), "7"); }
  { EXPECT_EQ(print_quoted("-123"), "-123"); }

  // Testing read of symbols
  { EXPECT_EQ(print_quoted("+"), "+"); }
  { EXPECT_EQ(print_quoted("abc"), "abc"); }
  { EXPECT_EQ(print_quoted("   abc   "), "abc"); }
  { EXPECT_EQ(print_quoted("abc5"), "abc5"); }
  { EXPECT_EQ(print_quoted("abc-def"), "abc-def"); }

  // Testing non-numbers starting with a dash.
  { EXPECT_EQ(print_quoted("-"), "-"); }
  { EXPECT_EQ(print_quoted("-abc"), "-abc"); }
  { EXPECT_EQ(print_quoted("->>"), "->>"); }

  // Testing read of lists
  { EXPECT_EQ(print_quoted("(+ 1 2)"), "(+ (1 2))"); }
  { EXPECT_EQ(print_quoted("()"), "()"); }
  { EXPECT_EQ(print_quoted("( )"), "()"); }
  { EXPECT_EQ(print_quoted("(nil)"), "nil"); }
  { EXPECT_EQ(print_quoted("((3 4))"), "(3 4)"); }
  { EXPECT_EQ(print_quoted("(+ 1 (+ 2 3))"), "(+ (1 (+ (2 3))))"); }
  {
    EXPECT_EQ(print_quoted("  ( +   1   (+   2 3   )   )  "),
              "(+ (1 (+ (2 3))))");
  }

  { EXPECT_EQ(print_quoted("(* 1 2)"), "(* (1 2))"); }
  { EXPECT_EQ(print_quoted("(** 1 2)"), "(** (1 2))"); }
  { EXPECT_EQ(print_quoted("(* -3 6)"), "(* (-3 6))"); }
  { EXPECT_EQ(print_quoted("(()())"), "(() ())"); }

  // Testing read of nil/true/false
  { EXPECT_EQ(print_quoted("nil"), "nil"); }
  { EXPECT_EQ(print_quoted("true"), "true"); }
  { EXPECT_EQ(print_quoted("false"), "false"); }

  // Testing read of strings
  { EXPECT_EQ(print_quoted(R"("abc")"), R"("abc")"); }
  { EXPECT_EQ(print_quoted(R"(   "abc"   )"), R"("abc")"); }
  {
    EXPECT_EQ(print_quoted(R"STR("abc (with parens)")STR"),
              R"STR("abc (with parens)")STR");
  }
  { EXPECT_EQ(print_quoted(R"("abc\"def")"), R"("abc\"def")"); }
  { EXPECT_EQ(print_quoted(R"("")"), R"("")"); }
  { EXPECT_EQ(print_quoted(R"("\\")"), R"("\\")"); }
  {
    EXPECT_EQ(print_quoted(R"("\\\\\\\\\\\\\\\\\\")"),
              R"("\\\\\\\\\\\\\\\\\\")");
  }

  { EXPECT_EQ(print_quoted(R"("&")"), R"("&")"); }
  { EXPECT_EQ(print_quoted(R"("'")"), R"("'")"); }
  { EXPECT_EQ(print_quoted(R"("(")"), R"("(")"); }
  { EXPECT_EQ(print_quoted(R"RAW_STR(")")RAW_STR"), R"RAW_STR(")")RAW_STR"); }
  { EXPECT_EQ(print_quoted(R"("*")"), R"("*")"); }
  { EXPECT_EQ(print_quoted(R"("+")"), R"("+")"); }
  { EXPECT_EQ(print_quoted(R"(",")"), R"(",")"); }
  { EXPECT_EQ(print_quoted(R"("-")"), R"("-")"); }
  { EXPECT_EQ(print_quoted(R"("/")"), R"("/")"); }
  { EXPECT_EQ(print_quoted(R"(":")"), R"(":")"); }
  { EXPECT_EQ(print_quoted(R"(";")"), R"(";")"); }
  { EXPECT_EQ(print_quoted(R"("<")"), R"("<")"); }
  { EXPECT_EQ(print_quoted(R"("=")"), R"("=")"); }
  { EXPECT_EQ(print_quoted(R"(">")"), R"(">")"); }
  { EXPECT_EQ(print_quoted(R"("?")"), R"("?")"); }
  { EXPECT_EQ(print_quoted(R"("@")"), R"("@")"); }
  { EXPECT_EQ(print_quoted(R"("[")"), R"("[")"); }
  { EXPECT_EQ(print_quoted(R"("]")"), R"("]")"); }
  { EXPECT_EQ(print_quoted(R"("^")"), R"("^")"); }
  { EXPECT_EQ(print_quoted(R"("_")"), R"("_")"); }
  { EXPECT_EQ(print_quoted(R"("`")"), R"("`")"); }
  { EXPECT_EQ(print_quoted(R"("{")"), R"("{")"); }
  { EXPECT_EQ(print_quoted(R"("}")"), R"("}")"); }
  { EXPECT_EQ(print_quoted(R"("~")"), R"("~")"); }
  { EXPECT_EQ(print_quoted(R"("!")"), R"("!")"); }

  // Testing read of quoting
  // { EXPECT_EQ(print_quoted(R"('1)"), R"((quote (1 nil)))"); }
  // {
  //   EXPECT_EQ(print_quoted(R"('(1 2 3))"), R"((quote ((1 (2 3)) nil)))");
  // }
  // { EXPECT_EQ(print_quoted(R"(`1)"), R"(1)"); }
  // { EXPECT_EQ(print_quoted(R"(`(1 2 3))"), R"((1 (2 3)))"); }
  // { EXPECT_EQ(print_quoted(R"(,1)"), R"((comma (1 nil)))"); }
  // {
  //   EXPECT_EQ(print_quoted(R"(,(1 2 3))"), R"((comma ((1 (2 3)) nil)))");
  // }
  // {
  //   EXPECT_EQ(print_quoted(R"(`(1 ,a 3))"), R"((1 ((comma (a nil)) 3)))");
  // }
  // {
  //   EXPECT_EQ(print_quoted(R"(,@(1 2 3))"),
  //                R"((commaat ((1 (2 3)) nil)))");
  // }
}
