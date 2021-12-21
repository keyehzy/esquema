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
    Cons* obj = Cons::cons(Expr::nil(), Expr::nil(), heap);
    (void)obj;
  }

  {
    Heap heap(8 * KiB);
    Procedure* obj = Procedure::proc(Expr::nil(), Expr::nil(), heap);
    (void)obj;
  }

  {
    Heap heap(8 * KiB);
    Procedure* obj =
        Procedure::proc(Atom("foo"_sv), Expr::nil(), Expr::nil(), heap);
    (void)obj;
  }

  {
    Heap heap(8 * KiB);
    Procedure* obj = Procedure::proc(Atom("foo"_sv), {}, heap);
    (void)obj;
  }
}

TEST(test_heap, use_heap_to_allocate_multiple_objects) {
  {
    Heap heap(8 * KiB);
    Cons* obj_1 = Cons::cons(Expr::nil(), Expr::nil(), heap);
    (void)obj_1;

    Procedure* obj_2 = Procedure::proc(Expr::nil(), Expr::nil(), heap);
    (void)obj_2;

    Procedure* obj_3 =
        Procedure::proc(Atom("foo"_sv), Expr::nil(), Expr::nil(), heap);
    (void)obj_3;

    Procedure* obj_4 = Procedure::proc(Atom("foo"_sv), {}, heap);
    (void)obj_4;
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
    Cons* obj = Cons::cons(Expr::nil(), Expr::nil(), heap);
    std::vector<Cons*> list;
    list.push_back(obj);
  }

  {
    Heap heap(8 * KiB);
    std::vector<Cons*> list;
    list.emplace_back(Cons::cons(Expr::nil(), Expr::nil(), heap));
  }

  {
    Heap heap(8 * KiB);
    std::vector<Expr> list;
    list.emplace_back(Expr(Cons::cons(Expr::nil(), Expr::nil(), heap)));
  }

  {
    Heap heap(8 * KiB);
    Procedure* obj = Procedure::proc(Expr::nil(), Expr::nil(), heap);
    std::vector<Procedure*> list;
    list.push_back(obj);
  }

  {
    Heap heap(8 * KiB);
    std::vector<Procedure*> list;
    list.emplace_back(Procedure::proc(Expr::nil(), Expr::nil(), heap));
  }

  {
    Heap heap(8 * KiB);
    std::vector<Expr> list;
    list.emplace_back(Expr(Procedure::proc(Expr::nil(), Expr::nil(), heap)));
  }

  {
    Heap heap(8 * KiB);
    std::vector<Expr> list;
    list.emplace_back(Expr(Cons::cons(Expr::nil(), Expr::nil(), heap)));
    list.emplace_back(Expr(Procedure::proc(Expr::nil(), Expr::nil(), heap)));
    list.emplace_back(
        Expr(Procedure::proc(Atom("foo"_sv), Expr::nil(), Expr::nil(), heap)));
    list.emplace_back(Expr(Procedure::proc(Atom("foo"_sv), {}, heap)));
  }
}

// TEST(test_heap, symbol_value_on_container) {
//   {
//     std::vector<symbol_value> list;
//     symbol_value sb{Atom("foo"_sv), Expr::nil()};
//     list.push_back(sb);
//   }

//   {
//     Heap heap(8 * KiB);
//     std::vector<symbol_value> list;
//     Cons* obj = Cons::cons(Expr::nil(), Expr::nil(), heap);
//     symbol_value sb{Atom("foo"_sv), Expr(obj)};
//     list.push_back(sb);
//   }

//   {
//     Heap heap(8 * KiB);
//     std::vector<symbol_value> list;
//     symbol_value sb{Atom("foo"_sv),
//                     Expr(Cons::cons(Expr::nil(), Expr::nil(), heap))};
//     list.push_back(sb);
//   }

//   {
//     std::vector<symbol_value> list;
//     symbol_value sb{Atom("foo"_sv), Expr::nil()};
//     list.emplace_back(sb);
//   }

//   {
//     Heap heap(8 * KiB);
//     std::vector<symbol_value> list;
//     Cons* obj = Cons::cons(Expr::nil(), Expr::nil(), heap);
//     symbol_value sb{Atom("foo"_sv), Expr(obj)};
//     list.emplace_back(sb);
//   }

//   {
//     Heap heap(8 * KiB);
//     std::vector<symbol_value> list;
//     symbol_value sb{Atom("foo"_sv),
//                     Expr(Cons::cons(Expr::nil(), Expr::nil(), heap))};
//     list.emplace_back(sb);
//   }

//   {
//     Heap heap(8 * KiB);
//     std::vector<symbol_value> list;
//     list.emplace_back(Atom("foo"_sv),
//                       Expr(Cons::cons(Expr::nil(), Expr::nil(), heap)));
//   }

//   {
//     Heap heap(8 * KiB);
//     std::vector<symbol_value> list;
//     list.emplace_back(Atom("foo"_sv),
//                       Expr(Cons::cons(Expr::nil(), Expr::nil(), heap)));
//     list.emplace_back(Atom("foo"_sv),
//                       Expr(Cons::cons(Expr::nil(), Expr::nil(), heap)));
//     list.emplace_back(Atom("foo"_sv),
//                       Expr(Cons::cons(Expr::nil(), Expr::nil(), heap)));
//     list.emplace_back(Atom("foo"_sv),
//                       Expr(Cons::cons(Expr::nil(), Expr::nil(), heap)));
//   }

//   {
//     Heap heap(8 * KiB);
//     std::vector<symbol_value> list;
//     list.emplace_back(Atom("foo"_sv),
//                       Expr(Cons::cons(Expr::nil(), Expr::nil(), heap)));
//     list.pop_back();
//   }

//   {
//     Heap heap(8 * KiB);
//     std::vector<symbol_value> list;
//     list.emplace_back(Atom("foo"_sv),
//                       Expr(Cons::cons(Expr::nil(), Expr::nil(), heap)));
//     list.pop_back();
//     list.emplace_back(Atom("foo"_sv),
//                       Expr(Cons::cons(Expr::nil(), Expr::nil(), heap)));
//     list.pop_back();
//     list.emplace_back(Atom("foo"_sv),
//                       Expr(Cons::cons(Expr::nil(), Expr::nil(), heap)));
//     list.pop_back();
//     list.emplace_back(Atom("foo"_sv),
//                       Expr(Cons::cons(Expr::nil(), Expr::nil(), heap)));
//     list.pop_back();
//     list.emplace_back(Atom("foo"_sv),
//                       Expr(Cons::cons(Expr::nil(), Expr::nil(), heap)));
//     list.pop_back();
//   }
// }
