#include <esquema/print.h>
#include <gtest/gtest.h>

string pprint(string_view sv) {
  printer p(sv);
  return p.print();
}

TEST(test, level2) {
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

  { EXPECT_EQ(pprint("(begin (define bar))"_sv), "bar"_sv); };  // TODO: we
  // need to differenciate between 'nothing' and nill

  {
    EXPECT_EQ(pprint("(begin (define add3 (lambda (x) (+ x 3))) (add3 3)) "_sv),
              "6"_sv);
  };

  { EXPECT_EQ(pprint("(begin (define x 1) (define (f x) 2) x)"_sv), "1"_sv); };

  {
    EXPECT_EQ(
        pprint(
            "(begin (define x 1) (define (f x) (g 2)) (define (g y) (+ x y)) (f 5)) "_sv),
        "3"_sv);
  };

  // // Testing define (second form)
  { EXPECT_EQ(pprint("(begin (define (f x) x) (f 1))"_sv), "1"_sv); };

  // // Testing define (closures)
  {
      // EXPECT_EQ(pprint("(begin (define (foo x y) (lambda (x) x)) (foo x
      // y))"_sv),
      //           "(closure ((x . x) (y . y) t) (x) x)"_sv);
  };
}

TEST(test, begin) {
  // Testing begin
  { EXPECT_EQ(pprint("(begin 1 2 3)"_sv), "3"_sv); }
  { EXPECT_EQ(pprint("(begin 1 2)"_sv), "2"_sv); }
  { EXPECT_EQ(pprint("(begin 1)"_sv), "1"_sv); }
  { EXPECT_EQ(pprint("(begin (define x 0) (set! x 5) (+ x 1))"_sv), "6"_sv); }
  { EXPECT_EQ(pprint("(begin (define x 0) (set! x 5) (+ x 1))"_sv), "6"_sv); }
  {
    EXPECT_EQ(
        pprint(
            "(begin (define count 0) (set! count (+ count 1)) (* count 3))"_sv),
        "3"_sv);
  }
}

TEST(test, quote) {  // Testing quote of lists
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

TEST(test, list) {
  { EXPECT_EQ(pprint("(list 1 2 3)"_sv), "(1 2 3)"_sv); }
  { EXPECT_EQ(pprint("(list (+ 1 2) 4 5)"_sv), "(3 4 5)"_sv); }
  { EXPECT_EQ(pprint("(list 'a (+ 3 4) 'c)"_sv), "(a 7 c)"_sv); }
  { EXPECT_EQ(pprint("(list)"_sv), "()"_sv); }
  {
    EXPECT_EQ(pprint("(begin (define x 42) (list (+ x 1) (+ x 2) (+ x 3)))"_sv),
              "(43 44 45)"_sv);
  }
  { EXPECT_EQ(pprint("(append '(1) '(2))"_sv), "(1 2)"_sv); }
  { EXPECT_EQ(pprint("(append '(1 2) '(3 4))"_sv), "(1 2 3 4)"_sv); }
}

TEST(test, quasiquote) {
  { EXPECT_EQ(pprint("`(+ 1 2)"_sv), "(+ 1 2)"_sv); }
  { EXPECT_EQ(pprint("`,(+ 1 2)"_sv), "3"_sv); }
  { EXPECT_EQ(pprint("`(+ 1 ,(+ 1 2))"_sv), "(+ 1 3)"_sv); }
  { EXPECT_EQ(pprint("`(list 1 2)"_sv), "(list 1 2)"_sv); }
  { EXPECT_EQ(pprint("`(list ,(+ 1 2) 4)"_sv), "(list 3 4)"_sv); }
  {
    EXPECT_EQ(pprint("(let ((name 'a)) `(list ,name ',name))"_sv),
              "(list a (quote a))"_sv);
  }
  {
    EXPECT_EQ(pprint("(let ((foo '(foo bar))) `(list ,@foo))"_sv),
              "(list foo bar)"_sv);
  }
}

TEST(test, if_statement) {
  // Testing if
  { EXPECT_EQ(pprint("(if #t 2 3)"_sv), "2"_sv); }
  { EXPECT_EQ(pprint("(if #f 2 3)"_sv), "3"_sv); }
  { EXPECT_EQ(pprint("(if (> 3 2) 'yes 'no)"_sv), "yes"_sv); }
  { EXPECT_EQ(pprint("(if (> 2 3) 'yes 'no)"_sv), "no"_sv); }
  { EXPECT_EQ(pprint("(if (> 3 2) (- 3 2) (+ 3 2))"_sv), "1"_sv); }
  { EXPECT_EQ(pprint("(if (>= 3 2) 'yes 'no)"_sv), "yes"_sv); }
  { EXPECT_EQ(pprint("(if (>= 3 3) 'yes 'no)"_sv), "yes"_sv); }
  { EXPECT_EQ(pprint("(if (>= 2 3) 'yes 'no)"_sv), "no"_sv); }
  { EXPECT_EQ(pprint("(if (<= 3 2) 'yes 'no)"_sv), "no"_sv); }
  { EXPECT_EQ(pprint("(if (<= 3 3) 'yes 'no)"_sv), "yes"_sv); }
  { EXPECT_EQ(pprint("(if (<= 2 3) 'yes 'no)"_sv), "yes"_sv); }
  { EXPECT_EQ(pprint("((if #f = *) 3 4)"_sv), "12"_sv); }
  { EXPECT_EQ(pprint("((if #t = *) 3 4)"_sv), "#f"_sv); }
}

TEST(test, examples) {
  {
    EXPECT_EQ(pprint("(lambda (x) (+ x x))"_sv),
              "(closure (t) (x) (+ x x))"_sv);
  }
  { EXPECT_EQ(pprint("((lambda (x) (+ x x)) 4)"_sv), "8"_sv); }
  {
    EXPECT_EQ(
        pprint(
            "(begin (define reverse-subtract (lambda (x y) (- y x))) (reverse-subtract 7 10))"_sv),
        "3"_sv);
  }

  {
    EXPECT_EQ(
        pprint(
            "(begin (define foo (let ((x 4)) (lambda (y) (+ x y)))) (foo 6))"_sv),
        "10"_sv);
  }
}

TEST(test, let) {
  { EXPECT_EQ(pprint("(let ((x 1)) x)"_sv), "1"_sv); }
  { EXPECT_EQ(pprint("(let ((x 1) (y 2)) x)"_sv), "1"_sv); }
  { EXPECT_EQ(pprint("(let ((x 1) (y 2)) y)"_sv), "2"_sv); }
  { EXPECT_EQ(pprint("(let ((x 1) (y x)) y)"_sv), "x"_sv); }  // TODO
  { EXPECT_EQ(pprint("(let ((x 2) (y 3)) (* x y))"_sv), "6"_sv); }
  {
    EXPECT_EQ(
        pprint(
            "(let ((x 5)) (define foo (lambda (y) (bar x y))) (define bar (lambda (a b) (+ (* a b) a))) (foo (+ x 3)))"_sv),
        "45"_sv);
  }  // autoquote
  {
    EXPECT_EQ(
        pprint(
            "(let ((x 2) (y 3)) (let ((foo (lambda (z) (+ x (+ y z)))) (x 7)) (foo 4)))"_sv),
        "9"_sv);
  }
}

TEST(test, let_star) {
  { EXPECT_EQ(pprint("(let* ((x 1) (y x)) y)"_sv), "1"_sv); }
  {
    EXPECT_EQ(
        pprint("(let ((x 2) (y 3)) (let* ((x 7) (z (+ x y))) (* z x)))"_sv),
        "70"_sv);
  }
}

TEST(test, letrec) {
  {
    EXPECT_EQ(
        pprint(
            "(letrec ((zero? (lambda (n) (= 0 n))) (even? (lambda (n) (if (zero? n) #t (odd? (- n 1))))) (odd? (lambda (n) (if (zero? n) #f (even? (- n 1)))))) (even? 88))"_sv),
        "#t"_sv);
  }
}

TEST(test, recursion) {
  {
    EXPECT_EQ(pprint("(begin (define (fib n) (if (<= n 2) 1 (+ (fib (- n 1))"
                     "(fib (- n 2))))) (fib 8))"),
              "21");

    EXPECT_EQ(pprint("(begin (define (fact n) (if (= n 1) 1 (* n (fact (- n 1) "
                     ")))) (fact 5))"),
              "120");
  }
}

TEST(test, car_cdr_cons) {
  { EXPECT_EQ(pprint("(car '(1 2))"_sv), "1"_sv); }
  { EXPECT_EQ(pprint("(car '((+ 1 2) 4))"_sv), "(+ 1 2)"_sv); }
  { EXPECT_EQ(pprint("(car (list 1 2))"_sv), "1"_sv); }
  { EXPECT_EQ(pprint("(car (list (+ 1 2) 4))"_sv), "3"_sv); }

  { EXPECT_EQ(pprint("(cdr '(1 2))"_sv), "2"_sv); }
  { EXPECT_EQ(pprint("(cdr '(4 (+ 1 2)))"_sv), "(+ 1 2)"_sv); }
  { EXPECT_EQ(pprint("(cdr (list 1 2))"_sv), "2"_sv); }
  { EXPECT_EQ(pprint("(cdr (list 4 (+ 1 2)))"_sv), "3"_sv); }

  { EXPECT_EQ(pprint("(car (cdr '((b) (x y) ((c)))))"_sv), "(x y)"_sv); }
  { EXPECT_EQ(pprint("(cdr (cdr '((b) (x y) ((c)))))"_sv), "c"_sv); }

  { EXPECT_EQ(pprint("(cons 'a '())"_sv), "a"_sv); }
  { EXPECT_EQ(pprint("(cons '(a b (c)) '())"_sv), "(a b c)"_sv); }

  { EXPECT_EQ(pprint("(cons 'a (car '((b) c d)))"_sv), "(a b)"_sv); }
  { EXPECT_EQ(pprint("(cons 'a (cdr '((b) c d))) "_sv), "(a c d)"_sv); }
}

TEST(test, predicates) { {{EXPECT_EQ(pprint("(boolean? #t)"_sv), "#t"_sv); }
{ EXPECT_EQ(pprint("(boolean? #f)"_sv), "#t"_sv); }
{ EXPECT_EQ(pprint("(boolean? 1)"_sv), "#f"_sv); }
{ EXPECT_EQ(pprint("(boolean? 'foo)"_sv), "#f"_sv); }
}

{
  { EXPECT_EQ(pprint("(null? '())"_sv), "#t"_sv); }
  { EXPECT_EQ(pprint("(null? (list))"_sv), "#t"_sv); }
  { EXPECT_EQ(pprint("(null? 2)"_sv), "#f"_sv); }
  { EXPECT_EQ(pprint("(null? 'foo)"_sv), "#f"_sv); }
}
}
