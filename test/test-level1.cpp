#include <esquema/print.h>
#include <gtest/gtest.h>

TEST(test, level1) {
  // Testing read of numbers
  { EXPECT_EQ(print_quoted("1"_sv), "1"_sv); }
  { EXPECT_EQ(print_quoted("7"_sv), "7"_sv); }
  { EXPECT_EQ(print_quoted("  7   "_sv), "7"_sv); }
  { EXPECT_EQ(print_quoted("-123"_sv), "-123"_sv); }

  // Testing read of symbols
  { EXPECT_EQ(print_quoted("+"_sv), "+"_sv); }
  { EXPECT_EQ(print_quoted("abc"_sv), "abc"_sv); }
  { EXPECT_EQ(print_quoted("   abc   "_sv), "abc"_sv); }
  { EXPECT_EQ(print_quoted("abc5"_sv), "abc5"_sv); }
  { EXPECT_EQ(print_quoted("abc-def"_sv), "abc-def"_sv); }

  // Testing non-numbers starting with a dash.
  { EXPECT_EQ(print_quoted("-"_sv), "-"_sv); }
  { EXPECT_EQ(print_quoted("-abc"_sv), "-abc"_sv); }
  { EXPECT_EQ(print_quoted("->>"_sv), "->>"_sv); }

  // Testing read of lists
  { EXPECT_EQ(print_quoted("(+ 1 2)"_sv), "(+ (1 2))"_sv); }
  { EXPECT_EQ(print_quoted("()"_sv), "()"_sv); }
  { EXPECT_EQ(print_quoted("( )"_sv), "()"_sv); }
  { EXPECT_EQ(print_quoted("(nil)"_sv), "nil"_sv); }
  { EXPECT_EQ(print_quoted("((3 4))"_sv), "(3 4)"_sv); }
  { EXPECT_EQ(print_quoted("(+ 1 (+ 2 3))"_sv), "(+ (1 (+ (2 3))))"_sv); }
  {
    EXPECT_EQ(print_quoted("  ( +   1   (+   2 3   )   )  "_sv),
              "(+ (1 (+ (2 3))))"_sv);
  }

  { EXPECT_EQ(print_quoted("(* 1 2)"_sv), "(* (1 2))"_sv); }
  { EXPECT_EQ(print_quoted("(** 1 2)"_sv), "(** (1 2))"_sv); }
  { EXPECT_EQ(print_quoted("(* -3 6)"_sv), "(* (-3 6))"_sv); }
  { EXPECT_EQ(print_quoted("(()())"_sv), "(() ())"_sv); }

  // Testing read of nil/true/false
  { EXPECT_EQ(print_quoted("nil"_sv), "nil"_sv); }
  { EXPECT_EQ(print_quoted("true"_sv), "true"_sv); }
  { EXPECT_EQ(print_quoted("false"_sv), "false"_sv); }

  // Testing read of strings
  { EXPECT_EQ(print_quoted(R"---("abc")---"), R"---("abc")---"); }
  { EXPECT_EQ(print_quoted(R"---(   "abc"   )---"), R"---("abc")---"); }
  {
    EXPECT_EQ(print_quoted(R"---("abc (with parens)")---"),
              R"---("abc (with parens)")---");
  }
  { EXPECT_EQ(print_quoted(R"---("abc\"def")---"), R"---("abc\"def")---"); }
  { EXPECT_EQ(print_quoted(R"---("")---"), R"---("")---"); }
  { EXPECT_EQ(print_quoted(R"---("\\")---"), R"---("\\")---"); }
  {
    EXPECT_EQ(print_quoted(R"---("\\\\\\\\\\\\\\\\\\")---"),
              R"---("\\\\\\\\\\\\\\\\\\")---");
  }

  { EXPECT_EQ(print_quoted(R"---("&")---"), R"---("&")---"); }
  { EXPECT_EQ(print_quoted(R"---("'")---"), R"---("'")---"); }
  { EXPECT_EQ(print_quoted(R"---("(")---"), R"---("(")---"); }
  { EXPECT_EQ(print_quoted(R"---(")")---"), R"---(")")---"); }
  { EXPECT_EQ(print_quoted(R"---("*")---"), R"---("*")---"); }
  { EXPECT_EQ(print_quoted(R"---("+")---"), R"---("+")---"); }
  { EXPECT_EQ(print_quoted(R"---(",")---"), R"---(",")---"); }
  { EXPECT_EQ(print_quoted(R"---("-")---"), R"---("-")---"); }
  { EXPECT_EQ(print_quoted(R"---("/")---"), R"---("/")---"); }
  { EXPECT_EQ(print_quoted(R"---(":")---"), R"---(":")---"); }
  { EXPECT_EQ(print_quoted(R"---(";")---"), R"---(";")---"); }
  { EXPECT_EQ(print_quoted(R"---("<")---"), R"---("<")---"); }
  { EXPECT_EQ(print_quoted(R"---("=")---"), R"---("=")---"); }
  { EXPECT_EQ(print_quoted(R"---(">")---"), R"---(">")---"); }
  { EXPECT_EQ(print_quoted(R"---("?")---"), R"---("?")---"); }
  { EXPECT_EQ(print_quoted(R"---("@")---"), R"---("@")---"); }
  { EXPECT_EQ(print_quoted(R"---("[")---"), R"---("[")---"); }
  { EXPECT_EQ(print_quoted(R"---("]")---"), R"---("]")---"); }
  { EXPECT_EQ(print_quoted(R"---("^")---"), R"---("^")---"); }
  { EXPECT_EQ(print_quoted(R"---("_")---"), R"---("_")---"); }
  { EXPECT_EQ(print_quoted(R"---("`")---"), R"---("`")---"); }
  { EXPECT_EQ(print_quoted(R"---("{")---"), R"---("{")---"); }
  { EXPECT_EQ(print_quoted(R"---("}")---"), R"---("}")---"); }
  { EXPECT_EQ(print_quoted(R"---("~")---"), R"---("~")---"); }
  { EXPECT_EQ(print_quoted(R"---("!")---"), R"---("!")---"); }

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
