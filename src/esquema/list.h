#include "assert.h"
#include "heap.h"
#include <functional>

template <typename T, typename U>
struct Node {
  Node() = delete;
  explicit Node(T key, U value) : entry(KeyValuePair{key, value}){};

  static void* operator new(size_t size, Heap& heap = arena) {
    return heap.allocate(size);
  }

  static void operator delete(void*) {}

  // Used for testing only
  static Node* env(T key, U value, Heap& heap) {
    return new (heap) Node(key, value);
  }

  struct KeyValuePair {
    T key;
    U value;
  };

  KeyValuePair entry{};
  Node* next = nullptr;
};

template <typename T, typename U>
class LinkedList {
 public:
  LinkedList() = default;

  // LinkedList(const LinkedList& list) {
  //   auto* it = list.m_root;
  //   while (it != nullptr) {
  //     this->add(it->entry.key, it->entry.value);
  //     it = it->next;
  //   }
  // }

  void add(T key, U value) {
    auto* node = new Node<T, U>(key, value);

    if (m_root) {
      m_last->next = node;
      m_last = node;
    } else {
      m_root = node;
      m_last = node;
    }

    m_size += 1;
  }

  Node<T, U>* get(int index) {
    ESQUEMA_ASSERT(index < m_size);
    auto* it = m_root;
    int pos = 0;
    while (it != nullptr && pos < index) {
      it = it->next;
      pos += 1;
    }
    return it;
  }

  Node<T, U>* find(T key) {
    auto* it = m_root;
    while (it != nullptr && it->entry.key != key) {
      it = it->next;
    }
    return it;
  }

  Node<T, U>* find_last(T key) {
    auto* it = m_root;
    Node<T, U>* last = nullptr;
    while (it != nullptr) {
      if (it->entry.key == key) {
        last = it;
      }
      it = it->next;
    }
    return last;
  }

  void extend_from(const LinkedList& env) {
    auto* it = env.m_root;
    while (it != nullptr) {
      this->add(it->entry.key, it->entry.value);
      it = it->next;
    }
  }

  void forEach(const std::function<void(const T&, const U&)>& operation) {
    auto* it = m_root;
    while (it != nullptr) {
      operation(it->entry.key, it->entry.value);
      it = it->next;
    }
  }

 private:
  Node<T, U>* m_root = nullptr;
  Node<T, U>* m_last = nullptr;
  int m_size = 0;
};
