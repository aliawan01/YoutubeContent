#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

enum BlockType {
  BLOCK_INT,
  BLOCK_FLOAT,
  BLOCK_DOUBLE,
  BLOCK_STRING
};

typedef struct Block {
  union {
    int int_val;
    float float_val;
    double double_val;
    char* string_val;
  };

  enum BlockType type;
  struct Block* next;
} Block;

typedef struct {
  Block* pool;
  Block* free_list;
  int count;
} PoolAllocator;

PoolAllocator pool_initialize(enum BlockType type, int size) {
  PoolAllocator pool = {
    .pool = malloc(sizeof(Block)*size),
    .count = size
  };

  for (int i = 0; i < size; i++) {
    pool.pool[i] = (Block) {
      .type = type,
      .next = NULL
    };

    pool.pool[i].next = NULL;
    if (pool.free_list == NULL) {
      pool.free_list = &pool.pool[i];
    }
    else {
      Block* block = pool.free_list;
      for (; block->next != NULL; block = block->next);
      block->next = &pool.pool[i];
    }
  }

  return pool;
}

Block* pool_allocate(PoolAllocator* pool) {
  if (pool->free_list == NULL) {
    return NULL;
  }

  Block* block = pool->free_list;
  pool->free_list = pool->free_list->next;
  return block;
}

void pool_free(PoolAllocator* pool, Block* block_to_add) {
  if (block_to_add == NULL) {
    return;
  }

  block_to_add->next = NULL;
  if (pool->free_list == NULL) {
    pool->free_list = block_to_add;
  }
  else {
    Block* block = pool->free_list;
    for (; block->next != NULL; block = block->next);
    block->next = block_to_add;
  }
}

int main(void) {
  PoolAllocator pool = pool_initialize(BLOCK_DOUBLE, 3);

  Block* first = pool_allocate(&pool);
  first->string_val = "hello world";
  printf("%s\n", first->string_val);

  return 0;
}
