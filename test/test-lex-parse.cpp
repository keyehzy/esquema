#include <esquema/print.h>
#include <gtest/gtest.h>

string pprint_quoted(string_view sv) {
  printer p(sv);
  return p.print_quoted();
}

TEST(test, level1) {
  // Testing read of numbers
  { EXPECT_EQ(pprint_quoted("1"_sv), "1"_sv); }
  { EXPECT_EQ(pprint_quoted("7"_sv), "7"_sv); }
  { EXPECT_EQ(pprint_quoted("  7   "_sv), "7"_sv); }
  { EXPECT_EQ(pprint_quoted("-123"_sv), "-123"_sv); }

  // Testing read of symbols
  { EXPECT_EQ(pprint_quoted("+"_sv), "+"_sv); }
  { EXPECT_EQ(pprint_quoted("abc"_sv), "abc"_sv); }
  { EXPECT_EQ(pprint_quoted("   abc   "_sv), "abc"_sv); }
  { EXPECT_EQ(pprint_quoted("abc5"_sv), "abc5"_sv); }
  { EXPECT_EQ(pprint_quoted("abc-def"_sv), "abc-def"_sv); }

  // Testing non-numbers starting with a dash.
  { EXPECT_EQ(pprint_quoted("-"_sv), "-"_sv); }
  { EXPECT_EQ(pprint_quoted("-abc"_sv), "-abc"_sv); }
  { EXPECT_EQ(pprint_quoted("->>"_sv), "->>"_sv); }

  // Testing read of lists
  { EXPECT_EQ(pprint_quoted("(+ 1 2)"_sv), "(+ 1 2)"_sv); }
  { EXPECT_EQ(pprint_quoted("()"_sv), "()"_sv); }
  { EXPECT_EQ(pprint_quoted("( )"_sv), "()"_sv); }
  { EXPECT_EQ(pprint_quoted("(nil)"_sv), "nil"_sv); }
  { EXPECT_EQ(pprint_quoted("((3 4))"_sv), "(3 4)"_sv); }
  { EXPECT_EQ(pprint_quoted("(+ 1 (+ 2 3))"_sv), "(+ 1 (+ 2 3))"_sv); }
  {
    EXPECT_EQ(pprint_quoted("  ( +   1   (+   2 3   )   )  "_sv),
              "(+ 1 (+ 2 3))"_sv);
  }

  { EXPECT_EQ(pprint_quoted("(* 1 2)"_sv), "(* 1 2)"_sv); }
  { EXPECT_EQ(pprint_quoted("(** 1 2)"_sv), "(** 1 2)"_sv); }
  { EXPECT_EQ(pprint_quoted("(* -3 6)"_sv), "(* -3 6)"_sv); }
  { EXPECT_EQ(pprint_quoted("(()())"_sv), "(() ())"_sv); }

  // Testing read of nil/true/false
  { EXPECT_EQ(pprint_quoted("nil"_sv), "nil"_sv); }
  { EXPECT_EQ(pprint_quoted("true"_sv), "true"_sv); }
  { EXPECT_EQ(pprint_quoted("false"_sv), "false"_sv); }

  // Testing read of strings
  { EXPECT_EQ(pprint_quoted(R"---("abc")---"), R"---("abc")---"); }
  { EXPECT_EQ(pprint_quoted(R"---(   "abc"   )---"), R"---("abc")---"); }
  {
    EXPECT_EQ(pprint_quoted(R"---("abc (with parens)")---"),
              R"---("abc (with parens)")---");
  }
  { EXPECT_EQ(pprint_quoted(R"---("abc\"def")---"), R"---("abc\"def")---"); }
  { EXPECT_EQ(pprint_quoted(R"---("")---"), R"---("")---"); }
  { EXPECT_EQ(pprint_quoted(R"---("\\")---"), R"---("\\")---"); }
  {
    EXPECT_EQ(pprint_quoted(R"---("\\\\\\\\\\\\\\\\\\")---"),
              R"---("\\\\\\\\\\\\\\\\\\")---");
  }

  { EXPECT_EQ(pprint_quoted(R"---("&")---"), R"---("&")---"); }
  { EXPECT_EQ(pprint_quoted(R"---("'")---"), R"---("'")---"); }
  { EXPECT_EQ(pprint_quoted(R"---("(")---"), R"---("(")---"); }
  { EXPECT_EQ(pprint_quoted(R"---(")")---"), R"---(")")---"); }
  { EXPECT_EQ(pprint_quoted(R"---("*")---"), R"---("*")---"); }
  { EXPECT_EQ(pprint_quoted(R"---("+")---"), R"---("+")---"); }
  { EXPECT_EQ(pprint_quoted(R"---(",")---"), R"---(",")---"); }
  { EXPECT_EQ(pprint_quoted(R"---("-")---"), R"---("-")---"); }
  { EXPECT_EQ(pprint_quoted(R"---("/")---"), R"---("/")---"); }
  { EXPECT_EQ(pprint_quoted(R"---(":")---"), R"---(":")---"); }
  { EXPECT_EQ(pprint_quoted(R"---(";")---"), R"---(";")---"); }
  { EXPECT_EQ(pprint_quoted(R"---("<")---"), R"---("<")---"); }
  { EXPECT_EQ(pprint_quoted(R"---("=")---"), R"---("=")---"); }
  { EXPECT_EQ(pprint_quoted(R"---(">")---"), R"---(">")---"); }
  { EXPECT_EQ(pprint_quoted(R"---("?")---"), R"---("?")---"); }
  { EXPECT_EQ(pprint_quoted(R"---("@")---"), R"---("@")---"); }
  { EXPECT_EQ(pprint_quoted(R"---("[")---"), R"---("[")---"); }
  { EXPECT_EQ(pprint_quoted(R"---("]")---"), R"---("]")---"); }
  { EXPECT_EQ(pprint_quoted(R"---("^")---"), R"---("^")---"); }
  { EXPECT_EQ(pprint_quoted(R"---("_")---"), R"---("_")---"); }
  { EXPECT_EQ(pprint_quoted(R"---("`")---"), R"---("`")---"); }
  { EXPECT_EQ(pprint_quoted(R"---("{")---"), R"---("{")---"); }
  { EXPECT_EQ(pprint_quoted(R"---("}")---"), R"---("}")---"); }
  { EXPECT_EQ(pprint_quoted(R"---("~")---"), R"---("~")---"); }
  { EXPECT_EQ(pprint_quoted(R"---("!")---"), R"---("!")---"); }

  // Testing read of quoting
  { EXPECT_EQ(pprint_quoted("'1"), "(quote 1)"); }
  { EXPECT_EQ(pprint_quoted("'(1 2 3)"), "(quote (1 2 3))"); }
  { EXPECT_EQ(pprint_quoted("(`1)"), "(quote 1)"); }
  {  // FIXME: spacing
    EXPECT_EQ(
        pprint_quoted("`(1 2 3)"),
        "(append (quote 1 )(append (quote 2 )(append (quote 3 )(quote ()))))");
  }
  // { EXPECT_EQ(pprint_quoted("`(+ 1 2)"), "(+ 1 2)"); }        // FIXME:
  // spacing
  { EXPECT_EQ(pprint_quoted("(,1)"), "(, 1)"); }              // FIXME: spacing
  { EXPECT_EQ(pprint_quoted("(,(1 2 3))"), "(, (1 2 3))"); }  // FIXME: spacing
  {
    EXPECT_EQ(
        pprint_quoted("(`(1 ,a 3))"),
        "(append (quote 1 )(append (list a)(append (quote 3 )(quote ()))))");
  }
  { EXPECT_EQ(pprint_quoted("(,@(1 2 3))"), "(,@ (1 2 3))"); }
  { EXPECT_EQ(pprint_quoted("`(,@(1 2 3))"), "(append (1 2 3)(quote ()))"); }
}
