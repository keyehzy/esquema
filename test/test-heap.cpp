#include "esquema/expr.h"
#include "esquema/heap.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

TEST(test_heap, initializing) {
  {
    Heap heap(8 * KiB);
    EXPECT_TRUE(heap.head() != nullptr);
    EXPECT_EQ(heap.size(), 8 * KiB);
    void* block = heap.allocate(42);
    EXPECT_TRUE(block);
    EXPECT_EQ(heap.head()->size, 42);
    heap.destroy();  // destructor call
    EXPECT_TRUE(heap.head() == nullptr);
  }

  {
    Heap heap(8 * KiB);
    EXPECT_TRUE(heap.head() != nullptr);
    EXPECT_EQ(heap.size(), 8 * KiB);

    for (int i = 0; i < 100; i++) {
      void* block = heap.allocate(42);
      EXPECT_TRUE(block);
    }

    Block* head = heap.head();
    for (int i = 0; i < 100; i++) {
      EXPECT_EQ(head->size, 42);
      head = head->next;
    }
    heap.destroy();  // destructor call
    EXPECT_TRUE(heap.head() == nullptr);
  }
}

TEST(test_heap, use_heap_to_allocate_objects) {
  {
    Heap heap(8 * KiB);

    EXPECT_TRUE(heap.head() != nullptr);
    EXPECT_EQ(heap.size(), 8 * KiB);

    Cons* obj = new Cons(Expr::nil(), Expr::nil());
    (void)obj;

    EXPECT_EQ(heap.head()->size, 8 * KiB);

    heap.destroy();  // destructor call
    EXPECT_TRUE(heap.head() == nullptr);
  }

  {
    Heap heap(8 * KiB);

    EXPECT_TRUE(heap.head() != nullptr);
    EXPECT_EQ(heap.size(), 8 * KiB);

    Procedure* obj = new Procedure(Expr::nil(), Expr::nil());
    (void)obj;

    EXPECT_EQ(heap.head()->size, 8 * KiB);

    heap.destroy();  // destructor call
    EXPECT_TRUE(heap.head() == nullptr);
  }

  {
    Heap heap(8 * KiB);

    EXPECT_TRUE(heap.head() != nullptr);
    EXPECT_EQ(heap.size(), 8 * KiB);

    Procedure* obj = new Procedure(Atom("foo"_sv), Expr::nil(), Expr::nil());
    (void)obj;

    heap.destroy();  // destructor call
    EXPECT_TRUE(heap.head() == nullptr);
  }

  {
    Heap heap(8 * KiB);

    EXPECT_TRUE(heap.head() != nullptr);
    EXPECT_EQ(heap.size(), 8 * KiB);

    Procedure* obj = new Procedure(Atom("foo"_sv), {});
    (void)obj;

    heap.destroy();  // destructor call
    EXPECT_TRUE(heap.head() == nullptr);
  }
}

TEST(test_heap, use_heap_to_allocate_multiple_objects) {
  {
    Heap heap(8 * KiB);

    EXPECT_TRUE(heap.head() != nullptr);
    EXPECT_EQ(heap.size(), 8 * KiB);

    Cons* obj_1 = new Cons(Expr::nil(), Expr::nil());
    (void)obj_1;

    Procedure* obj_2 = new Procedure(Expr::nil(), Expr::nil());
    (void)obj_2;

    Procedure* obj_3 = new Procedure(Atom("foo"_sv), Expr::nil(), Expr::nil());
    (void)obj_3;

    Procedure* obj_4 = new Procedure(Atom("foo"_sv), {});
    (void)obj_4;

    heap.destroy();  // destructor call
    EXPECT_TRUE(heap.head() == nullptr);
  }
}

TEST(test_heap, DISABLED_oom) {
  {
    Heap heap(1);
    heap.destroy();
    heap.allocate(1);
  }

  {
    Heap heap(0);
    heap.allocate(1);
  }

  {
    Heap heap(1);
    heap.allocate(1);
    heap.allocate(1);
  }
}

TEST(test_heap, object_inside_container) {
  {
    Heap heap(8 * KiB);
    Cons* obj = new Cons(Expr::nil(), Expr::nil());
    std::vector<Cons*> list;
    list.push_back(obj);
    heap.destroy();  // destructor call
    EXPECT_TRUE(heap.head() == nullptr);
  }

  {
    Heap heap(8 * KiB);
    std::vector<Cons*> list;
    list.emplace_back(new Cons(Expr::nil(), Expr::nil()));
    heap.destroy();  // destructor call
    EXPECT_TRUE(heap.head() == nullptr);
  }

  {
    Heap heap(8 * KiB);
    std::vector<Expr> list;
    list.emplace_back(Expr(new Cons(Expr::nil(), Expr::nil())));
    heap.destroy();  // destructor call
    EXPECT_TRUE(heap.head() == nullptr);
  }

  {
    Heap heap(8 * KiB);
    Procedure* obj = new Procedure(Expr::nil(), Expr::nil());
    std::vector<Procedure*> list;
    list.push_back(obj);
    heap.destroy();  // destructor call
    EXPECT_TRUE(heap.head() == nullptr);
  }

  {
    Heap heap(8 * KiB);
    std::vector<Procedure*> list;
    list.emplace_back(new Procedure(Expr::nil(), Expr::nil()));
    heap.destroy();  // destructor call
    EXPECT_TRUE(heap.head() == nullptr);
  }

  {
    Heap heap(8 * KiB);
    std::vector<Expr> list;
    list.emplace_back(Expr(new Procedure(Expr::nil(), Expr::nil())));
    heap.destroy();  // destructor call
    EXPECT_TRUE(heap.head() == nullptr);
  }

  {
    Heap heap(8 * KiB);
    std::vector<Expr> list;
    list.emplace_back(Expr(new Cons(Expr::nil(), Expr::nil())));
    list.emplace_back(Expr(new Procedure(Expr::nil(), Expr::nil())));
    list.emplace_back(
        Expr(new Procedure(Atom("foo"_sv), Expr::nil(), Expr::nil())));
    list.emplace_back(Expr(new Procedure(Atom("foo"_sv), {})));
    heap.destroy();  // destructor call
    EXPECT_TRUE(heap.head() == nullptr);
  }
}

TEST(test_heap, symbol_value_on_container) {
  {
    Heap heap(8 * KiB);
    std::vector<symbol_value> list;
    symbol_value sb{Atom("foo"_sv), Expr::nil()};
    list.push_back(sb);
    heap.destroy();  // destructor call
    EXPECT_TRUE(heap.head() == nullptr);
  }

  {
    Heap heap(8 * KiB);
    std::vector<symbol_value> list;
    Cons* obj = new Cons(Expr::nil(), Expr::nil());
    symbol_value sb{Atom("foo"_sv), Expr(obj)};
    list.push_back(sb);
    heap.destroy();  // destructor call
    EXPECT_TRUE(heap.head() == nullptr);
  }

  {
    Heap heap(8 * KiB);
    std::vector<symbol_value> list;
    symbol_value sb{Atom("foo"_sv), Expr(new Cons(Expr::nil(), Expr::nil()))};
    list.push_back(sb);
    heap.destroy();  // destructor call
    EXPECT_TRUE(heap.head() == nullptr);
  }

  {
    Heap heap(8 * KiB);
    std::vector<symbol_value> list;
    symbol_value sb{Atom("foo"_sv), Expr::nil()};
    list.emplace_back(sb);
    heap.destroy();  // destructor call
    EXPECT_TRUE(heap.head() == nullptr);
  }

  {
    Heap heap(8 * KiB);
    std::vector<symbol_value> list;
    Cons* obj = new Cons(Expr::nil(), Expr::nil());
    symbol_value sb{Atom("foo"_sv), Expr(obj)};
    list.emplace_back(sb);
    heap.destroy();  // destructor call
    EXPECT_TRUE(heap.head() == nullptr);
  }

  {
    Heap heap(8 * KiB);
    std::vector<symbol_value> list;
    symbol_value sb{Atom("foo"_sv), Expr(new Cons(Expr::nil(), Expr::nil()))};
    list.emplace_back(sb);
    heap.destroy();  // destructor call
    EXPECT_TRUE(heap.head() == nullptr);
  }

  {
    Heap heap(8 * KiB);
    std::vector<symbol_value> list;
    list.emplace_back(Atom("foo"_sv), Expr(new Cons(Expr::nil(), Expr::nil())));
    heap.destroy();  // destructor call
    EXPECT_TRUE(heap.head() == nullptr);
  }

  {
    Heap heap(8 * KiB);
    std::vector<symbol_value> list;
    list.emplace_back(Atom("foo"_sv), Expr(new Cons(Expr::nil(), Expr::nil())));
    list.emplace_back(Atom("foo"_sv), Expr(new Cons(Expr::nil(), Expr::nil())));
    list.emplace_back(Atom("foo"_sv), Expr(new Cons(Expr::nil(), Expr::nil())));
    list.emplace_back(Atom("foo"_sv), Expr(new Cons(Expr::nil(), Expr::nil())));
    heap.destroy();  // destructor call
    EXPECT_TRUE(heap.head() == nullptr);
  }

  {
    Heap heap(8 * KiB);
    std::vector<symbol_value> list;
    list.emplace_back(Atom("foo"_sv), Expr(new Cons(Expr::nil(), Expr::nil())));
    list.pop_back();
    heap.destroy();  // destructor call
    EXPECT_TRUE(heap.head() == nullptr);
  }

  {
    Heap heap(8 * KiB);
    std::vector<symbol_value> list;
    list.emplace_back(Atom("foo"_sv), Expr(new Cons(Expr::nil(), Expr::nil())));
    list.pop_back();
    list.emplace_back(Atom("foo"_sv), Expr(new Cons(Expr::nil(), Expr::nil())));
    list.pop_back();
    list.emplace_back(Atom("foo"_sv), Expr(new Cons(Expr::nil(), Expr::nil())));
    list.pop_back();
    list.emplace_back(Atom("foo"_sv), Expr(new Cons(Expr::nil(), Expr::nil())));
    list.pop_back();
    list.emplace_back(Atom("foo"_sv), Expr(new Cons(Expr::nil(), Expr::nil())));
    list.pop_back();
    heap.destroy();  // destructor call
    EXPECT_TRUE(heap.head() == nullptr);
  }
}
