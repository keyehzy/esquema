#include <esquema/print.h>
#include <gtest/gtest.h>

TEST(test, level2) {
  // Testing quote of lists
  { EXPECT_EQ(print("'(+ 1 2)"_sv), "(+ 1 2)"_sv); }
  { EXPECT_EQ(print("'()"_sv), "()"_sv); }
  { EXPECT_EQ(print("'( )"_sv), "()"_sv); }
  { EXPECT_EQ(print("'(nil)"_sv), "nil"_sv); }
  { EXPECT_EQ(print("'((3 4))"_sv), "(3 4)"_sv); }
  { EXPECT_EQ(print("'(+ 1 (+ 2 3))"_sv), "(+ 1 (+ 2 3))"_sv); }
  {
    EXPECT_EQ(print("'  ( +   1   (+   2 3   )   )  "_sv), "(+ 1 (+ 2 3))"_sv);
  }

  // Testing if
  { EXPECT_EQ(print("(if #t 2 3)"_sv), "2"_sv); }
  { EXPECT_EQ(print("(if #f 2 3)"_sv), "3"_sv); }

  // Testing begin
  { EXPECT_EQ(print("(begin 1 2 3)"_sv), "3"_sv); }
  { EXPECT_EQ(print("(begin 1 2)"_sv), "2"_sv); }
  { EXPECT_EQ(print("(begin 1)"_sv), "1"_sv); }

  // Testing set!
  // { EXPECT_EQ(print("(begin (set! x 42) x)"_sv), "42"_sv); }
  // { EXPECT_EQ(print("(begin (set! x 42) y)"_sv), "y"_sv); }  // autoquote

  // Testing binary operation
  { EXPECT_EQ(print("(+ 1 2)"_sv), "3"_sv); }
  { EXPECT_EQ(print("(- 1 2)"_sv), "-1"_sv); }
  { EXPECT_EQ(print("(* 2 3)"_sv), "6"_sv); }
  { EXPECT_EQ(print("(/ 6 2)"_sv), "3"_sv); }
  { EXPECT_EQ(print("(% 5 3)"_sv), "2"_sv); }

  { EXPECT_EQ(print("(+ 1.0 0.5)"_sv), "1.500000"_sv); }
  { EXPECT_EQ(print("(- 1.5 2.5)"_sv), "-1.000000"_sv); }
  { EXPECT_EQ(print("(* 2.5 2)"_sv), "5.000000"_sv); }
  { EXPECT_EQ(print("(/ 6.0 1.5)"_sv), "4.000000"_sv); }
  { EXPECT_EQ(print("(% 1.5 1.0)"_sv), "0.500000"_sv); }

  // Testing lambdas
  { EXPECT_EQ(print("((lambda (a) a) 1)"_sv), "1"_sv); }
  { EXPECT_EQ(print("((lambda (a) (+ a 1)) 1)"_sv), "2"_sv); }
  { EXPECT_EQ(print("((lambda (a) foo (+ a 1)) 1)"_sv), "2"_sv); }
  { EXPECT_EQ(print("((lambda (x y) (* x y)) 2 3)"_sv), "6"_sv); }
  { EXPECT_EQ(print("((lambda (a) ((lambda (b) (+ a b)) 1)) 2)"_sv), "3"_sv); }
  // {
  //   EXPECT_EQ(print("(begin (set! x (lambda (a) (+ a 1))) (x 41))"_sv),
  //             "42"_sv);
  // }
  { EXPECT_EQ(print("(lambda (a) a)"_sv), "(closure (t) (a) a)"_sv); }
  {
    EXPECT_EQ(print("(lambda (a b) (+ a b))"_sv),
              "(closure (t) (a b) (+ a b))"_sv);
  }

  // Testing named-lambdas
  { EXPECT_EQ(print("((named-lambda (f a) a) 1)"_sv), "1"_sv); }
  { EXPECT_EQ(print("((named-lambda (f a) (+ a 1)) 1)"_sv), "2"_sv); }
  { EXPECT_EQ(print("((named-lambda (f a) foo (+ a 1)) 1)"_sv), "2"_sv); }
  { EXPECT_EQ(print("((named-lambda (f x y) (* x y)) 2 3)"_sv), "6"_sv); }
  { EXPECT_EQ(print("(named-lambda (f a) a)"_sv), "(closure f (t) (a) a)"_sv); }

  // Testing define (second form)
  { EXPECT_EQ(print("(begin (define (f x) x) (f 1))"_sv), "1"_sv); };
  // { EXPECT_EQ(print("(begin (define x 1) (define (f x) (g 2)) (define (g y)
  // (+ x y)) (f 5))"_sv), "3"_sv); }
}
