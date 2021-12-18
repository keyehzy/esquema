#include <esquema/print.h>
#include <gtest/gtest.h>

string pprint(string_view sv) {
  printer p(sv);
  return p.print();
}

TEST(test, level2) {
  // Testing begin
  { EXPECT_EQ(pprint("(begin 1 2 3)"_sv), "3"_sv); }
  { EXPECT_EQ(pprint("(begin 1 2)"_sv), "2"_sv); }
  { EXPECT_EQ(pprint("(begin 1)"_sv), "1"_sv); }

  // Testing set!
  // { EXPECT_EQ(pprint("(begin (set! x 42) x)"_sv), "42"_sv); }
  // { EXPECT_EQ(pprint("(begin (set! x 42) y)"_sv), "y"_sv); }  // autoquote

  // Testing binary operation
  { EXPECT_EQ(pprint("(+ 1 2)"_sv), "3"_sv); }
  { EXPECT_EQ(pprint("(- 1 2)"_sv), "-1"_sv); }
  { EXPECT_EQ(pprint("(* 2 3)"_sv), "6"_sv); }
  { EXPECT_EQ(pprint("(/ 6 2)"_sv), "3"_sv); }
  { EXPECT_EQ(pprint("(% 5 3)"_sv), "2"_sv); }

  { EXPECT_EQ(pprint("(+ 1.0 0.5)"_sv), "1.500000"_sv); }
  { EXPECT_EQ(pprint("(- 1.5 2.5)"_sv), "-1.000000"_sv); }
  { EXPECT_EQ(pprint("(* 2.5 2)"_sv), "5.000000"_sv); }
  { EXPECT_EQ(pprint("(/ 6.0 1.5)"_sv), "4.000000"_sv); }
  { EXPECT_EQ(pprint("(% 1.5 1.0)"_sv), "0.500000"_sv); }

  // Testing lambdas
  { EXPECT_EQ(pprint("((lambda (a) a) 1)"_sv), "1"_sv); }
  { EXPECT_EQ(pprint("((lambda (a) (+ a 1)) 1)"_sv), "2"_sv); }
  { EXPECT_EQ(pprint("((lambda (a) foo (+ a 1)) 1)"_sv), "2"_sv); }
  { EXPECT_EQ(pprint("((lambda (x y) (* x y)) 2 3)"_sv), "6"_sv); }
  { EXPECT_EQ(pprint("((lambda (a) ((lambda (b) (+ a b)) 1)) 2)"_sv), "3"_sv); }
  // {
  //   EXPECT_EQ(pprint("(begin (set! x (lambda (a) (+ a 1))) (x 41))"_sv),
  //             "42"_sv);
  // }
  { EXPECT_EQ(pprint("(lambda (a) a)"_sv), "(closure (t) (a) a)"_sv); }
  {
    EXPECT_EQ(pprint("(lambda (a b) (+ a b))"_sv),
              "(closure (t) (a b) (+ a b))"_sv);
  }

  // Testing named-lambdas
  { EXPECT_EQ(pprint("((named-lambda (f a) a) 1)"_sv), "1"_sv); }
  { EXPECT_EQ(pprint("((named-lambda (f a) (+ a 1)) 1)"_sv), "2"_sv); }
  { EXPECT_EQ(pprint("((named-lambda (f a) foo (+ a 1)) 1)"_sv), "2"_sv); }
  { EXPECT_EQ(pprint("((named-lambda (f x y) (* x y)) 2 3)"_sv), "6"_sv); }
  {
    EXPECT_EQ(pprint("(named-lambda (f a) a)"_sv), "(closure f (t) (a) a)"_sv);
  }

  // Testing define (first form)
  { EXPECT_EQ(pprint("(begin (define x 1) x)"_sv), "1"_sv); };

  // { EXPECT_EQ(pprint("(begin (define bar))"_sv), "bar"_sv); }; // TODO: we
  // need to differenciate between 'nothing' and nill

  {
    EXPECT_EQ(pprint("(begin (define add3 (lambda (x) (+ x 3))) (add3 3))"_sv),
              "6"_sv);
  };
  {
    EXPECT_EQ(
        pprint(
            "(begin (define x 1) (define (f x) (g 2)) (define (g y) (+ x y)) (f 5))"_sv),
        "3"_sv);
  };

  // Testing define (second form)
  { EXPECT_EQ(pprint("(begin (define (f x) x) (f 1))"_sv), "1"_sv); };

  // Testing define (closures)
  {
    EXPECT_EQ(pprint("(begin (define (foo x y) (lambda (x) x)) (foo x y))"_sv),
              "(closure ((x . x) (y . y) t) (x) x)"_sv);
  };
}

TEST(test, quote) {
  // Testing quote of lists
  { EXPECT_EQ(pprint("(quote a)"_sv), "a"_sv); }
  { EXPECT_EQ(pprint("(quote (+ 1 2))"_sv), "(+ 1 2)"_sv); }
  { EXPECT_EQ(pprint("'a"_sv), "a"_sv); }
  { EXPECT_EQ(pprint("'(+ 1 2)"_sv), "(+ 1 2)"_sv); }
  { EXPECT_EQ(pprint("'(quote a)"_sv), "(quote a)"_sv); }
  { EXPECT_EQ(pprint("''a"_sv), "(quote a)"_sv); }
  { EXPECT_EQ(pprint("'\"abc\""_sv), "\"abc\""_sv); }
  { EXPECT_EQ(pprint("\"abc\""_sv), "\"abc\""_sv); }
  { EXPECT_EQ(pprint("'145932"_sv), "145932"_sv); }
  { EXPECT_EQ(pprint("145932"_sv), "145932"_sv); }
  { EXPECT_EQ(pprint("'#t"_sv), "#t"_sv); }
  { EXPECT_EQ(pprint("#t"_sv), "#t"_sv); }
  { EXPECT_EQ(pprint(R"('#\a)"), R"(#\a)"); }
  { EXPECT_EQ(pprint(R"(#\a)"), R"(#\a)"); }
  { EXPECT_EQ(pprint("'()"_sv), "()"_sv); }
  { EXPECT_EQ(pprint("'( )"_sv), "()"_sv); }
  { EXPECT_EQ(pprint("'(nil)"_sv), "nil"_sv); }
  { EXPECT_EQ(pprint("'((3 4))"_sv), "(3 4)"_sv); }
  { EXPECT_EQ(pprint("'(+ 1 (+ 2 3))"_sv), "(+ 1 (+ 2 3))"_sv); }
}

TEST(test, if_statement) {
  // Testing if
  { EXPECT_EQ(pprint("(if #t 2 3)"_sv), "2"_sv); }
  { EXPECT_EQ(pprint("(if #f 2 3)"_sv), "3"_sv); }
  { EXPECT_EQ(pprint("(if (> 3 2) 'yes 'no)"_sv), "yes"_sv); }
  { EXPECT_EQ(pprint("(if (> 2 3) 'yes 'no)"_sv), "no"_sv); }
  { EXPECT_EQ(pprint("(if (> 3 2) (- 3 2) (+ 3 2))"_sv), "1"_sv); }
}
}
