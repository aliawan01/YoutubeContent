#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef int64_t i64;
typedef int32_t i32;
typedef int16_t i16;
typedef int8_t i8;

typedef double f64;
typedef float f32;

typedef u32 b32;

#define true 1
#define false 0

typedef struct {
  i32 max_value;
  u32* sparse;
  u32* dense;
  u32 count;
  u32 max_count;
  i32 offset;
} SparseSet;

SparseSet sparse_set_init(i32 min_value, i32 max_value, i32 capacity) {
  SparseSet sparse_set = {0};

  if (min_value > max_value) {
    printf("[SPARSE_SET_INIT] Error should be max_value > min_value\n");
    return sparse_set;
  }

  sparse_set.max_value = max_value-min_value;
  sparse_set.offset = -min_value;
  sparse_set.max_count = capacity;

  sparse_set.dense = malloc(sizeof(u32)*sparse_set.max_count);
  sparse_set.sparse = malloc(sizeof(u32)*sparse_set.max_value);

  return sparse_set;
}

b32 __sparse_set_value_exists(SparseSet sparse_set, i32 value) {
  if (value >= sparse_set.max_value || value < 0)
    return false;

  u32 index = sparse_set.sparse[value];
  
  return (index < sparse_set.count && sparse_set.dense[index] == value);
}

b32 sparse_set_value_exists(SparseSet sparse_set, i32 value) {
  value += sparse_set.offset;
  
  return __sparse_set_value_exists(sparse_set, value);
}

b32 sparse_set_insert(SparseSet* sparse_set, i32 value) {
  value += sparse_set->offset;

  if (!__sparse_set_value_exists(*sparse_set, value) && sparse_set->count < sparse_set->max_count) {
    sparse_set->dense[sparse_set->count] = value;
    sparse_set->sparse[value] = sparse_set->count;
    sparse_set->count++;
    return true;
  }

  return false;
}

b32 sparse_set_delete(SparseSet* sparse_set, i32 value) {
  value += sparse_set->offset;

  if (__sparse_set_value_exists(*sparse_set, value)) {
    u32 index = sparse_set->sparse[value];
    sparse_set->dense[index] = sparse_set->dense[--sparse_set->count];
    sparse_set->sparse[sparse_set->dense[index]] = index;

    return true;
  }

  return false;
}

void sparse_set_clear(SparseSet* sparse_set) {
  sparse_set->count = 0;
}

void sparse_set_free(SparseSet sparse_set) {
  free(sparse_set.dense);
  free(sparse_set.sparse);
}

int main(void) {
  SparseSet sparse_set = sparse_set_init(-20, 30, 5);

  printf("insert result: %d\n", sparse_set_insert(&sparse_set, -5));
  printf("insert result: %d\n", sparse_set_insert(&sparse_set, 4));
  printf("insert result: %d\n", sparse_set_insert(&sparse_set, -3));
  printf("insert result: %d\n", sparse_set_insert(&sparse_set, -3));
  printf("insert result: %d\n", sparse_set_insert(&sparse_set, 8));
  printf("insert result: %d\n", sparse_set_insert(&sparse_set, 9));
  printf("insert result: %d\n", sparse_set_insert(&sparse_set, -50));
  printf("insert result: %d\n", sparse_set_insert(&sparse_set, 6));
  printf("Exists %d: %d\n", -5, sparse_set_value_exists(sparse_set, -5));

  printf("delete result: %d\n", sparse_set_delete(&sparse_set, -5));
  printf("Exists %d: %d\n", -5, sparse_set_value_exists(sparse_set, -5));
  printf("delete result: %d\n", sparse_set_delete(&sparse_set, 6));

  printf("------\nAfter Clear\n------\n");
  sparse_set_clear(&sparse_set);
  printf("insert result: %d\n", sparse_set_insert(&sparse_set, 7));
  printf("insert result: %d\n", sparse_set_insert(&sparse_set, 2));
  printf("Exists %d: %d\n", 7, sparse_set_value_exists(sparse_set, 7));
  printf("Exists %d: %d\n", 2, sparse_set_value_exists(sparse_set, 2));
  printf("Exists %d: %d\n", 8, sparse_set_value_exists(sparse_set, 8));


  sparse_set_free(sparse_set);


  return 0;
}
