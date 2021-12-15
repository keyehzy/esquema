#include <esquema/print.h>
#include <gtest/gtest.h>

TEST(test, level2) {
  // Testing quote of lists
  { EXPECT_EQ(print("'(+ 1 2)"), "(+ (1 2))"); }
  { EXPECT_EQ(print("'()"), "()"); }
  { EXPECT_EQ(print("'( )"), "()"); }
  { EXPECT_EQ(print("'(nil)"), "nil"); }
  { EXPECT_EQ(print("'((3 4))"), "(3 4)"); }
  { EXPECT_EQ(print("'(+ 1 (+ 2 3))"), "(+ (1 (+ (2 3))))"); }
  { EXPECT_EQ(print("'  ( +   1   (+   2 3   )   )  "), "(+ (1 (+ (2 3))))"); }

  // Testing if
  { EXPECT_EQ(print("(if #t 2 3)"), "2"); }
  { EXPECT_EQ(print("(if #f 2 3)"), "3"); }

  // Testing begin
  { EXPECT_EQ(print("(begin 1 2 3)"), "3"); }
  { EXPECT_EQ(print("(begin 1 2)"), "2"); }
  { EXPECT_EQ(print("(begin 1)"), "1"); }

  // Testing set!
  { EXPECT_EQ(print("(begin (set! x 42) x)"), "42"); }
  { EXPECT_EQ(print("(begin (set! x 42) y)"), "y"); }  // autoquote

  // Testing binary operation
  { EXPECT_EQ(print("(+ 1 2)"), "3"); }
  { EXPECT_EQ(print("(- 1 2)"), "-1"); }
  { EXPECT_EQ(print("(* 2 3)"), "6"); }
  { EXPECT_EQ(print("(/ 6 2)"), "3"); }
  { EXPECT_EQ(print("(% 5 3)"), "2"); }

  { EXPECT_EQ(print("(+ 1.0 0.5)"), "1.500000"); }
  { EXPECT_EQ(print("(- 1.5 2.5)"), "-1.000000"); }
  { EXPECT_EQ(print("(* 2.5 2)"), "5.000000"); }
  { EXPECT_EQ(print("(/ 6.0 1.5)"), "4.000000"); }
  { EXPECT_EQ(print("(% 1.5 1.0)"), "0.500000"); }
}
