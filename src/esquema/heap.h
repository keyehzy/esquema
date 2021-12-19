#pragma once

#include "assert.h"
#include <cstdint>

static constexpr size_t MAX_HEAP_SIZE = 64 * 1024;
static constexpr size_t KiB = 1024;
static constexpr size_t MiB = 1024 * 1024;

struct Block {
  uint32_t size;
  bool free;
  Block* next;
};

class Heap {
 public:
  explicit Heap(uint32_t size);
  ~Heap();

  void* allocate(std::size_t size);
  void destroy();
  uint32_t size() const { return m_size; }
  Block* head() const { return m_head; }

 private:
  uint32_t m_size;
  Block* m_freelist;
  Block* m_head;
  char* m_memory;
  bool m_malloced;
};
