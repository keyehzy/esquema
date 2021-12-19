#include "esquema/assert.h"
#include "esquema/heap.h"
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>

Heap::Heap(uint32_t size) : m_size(size) {
  // Init memory
  void* block = std::malloc(m_size);
  ESQUEMA_ASSERT(block);
  std::memset(block, 0, m_size);
  m_memory = (char*)block;
  m_malloced = true;

  // Set head
  m_head = reinterpret_cast<Block*>(&m_memory[0]);
  m_head->next = nullptr;
  m_head->size = m_size;
  m_head->free = true;
  m_freelist = m_head;
}

Heap::~Heap() { this->destroy(); }

void Heap::destroy() {
  if (m_malloced) {
    free(m_head);
    m_head = nullptr;
    m_malloced = false;
  }
}

void* Heap::allocate(std::size_t size) {
  Block* free_cell = m_freelist;

  ESQUEMA_ASSERT(free_cell && free_cell->free);

  void* new_block = free_cell + 1;
  std::memset(new_block, 0, size);
  ESQUEMA_ASSERT(new_block);

  free_cell->size = size;
  free_cell->free = false;

  // OOM
  ESQUEMA_ASSERT((size_t)new_block + size < (size_t)m_head + m_size);
  size_t block_size =
      ((size_t)m_head + m_size) - ((size_t)new_block + size + 1);
  ESQUEMA_ASSERT(block_size > sizeof(Block));
  Block* next_block = reinterpret_cast<Block*>((char*)new_block + size);
  next_block->size = block_size - sizeof(Block);
  next_block->free = true;
  next_block->next = nullptr;
  free_cell->next = next_block;

  m_freelist = m_freelist->next;
  return new_block;
}
