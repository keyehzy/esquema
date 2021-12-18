#include <esquema/print.h>
#include <gtest/gtest.h>

TEST(test, chapter2) {
  auto pprint = [](string_view sv) {
    printer p(sv);
    return p.print();
  };

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
}
