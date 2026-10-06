#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
  int count;
  int max_count;
} DynArrayHeader;

#define dyn_array_header(a) (((DynArrayHeader*)(a))-1)

#define dyn_array_push(a, v) { \
  dyn_array_maybe_allocate_or_grow((void**)&a, sizeof(*(a))); \
  (a)[dyn_array_header((a))->count++] = (v); \
}

#define dyn_array_len(a) ((a) ? dyn_array_header((a))->count : -1)
#define dyn_array_pop(a) if ((a) && dyn_array_header((a))->count > 0) dyn_array_header((a))->count--

void dyn_array_maybe_allocate_or_grow(void** array, int elem_size) {
  int init_capacity = 5;

  if (!(*array)) {
    *array = malloc(elem_size*init_capacity + sizeof(DynArrayHeader));
    *array = (((DynArrayHeader*)*array)+1);

    DynArrayHeader* header = dyn_array_header(*array);
    header->count = 0;
    header->max_count = init_capacity;
  }

  DynArrayHeader* header = dyn_array_header(*array);
  if (header->count+1 == header->max_count) {
    header->max_count *= 2;
    printf("realloc new max size: %d\n", header->max_count);
    header = realloc(header, elem_size*header->max_count + sizeof(DynArrayHeader));
    *array = header+1;
  }
}


int main(void) {
  char* array = NULL;

  dyn_array_push(array, 'a');
  dyn_array_push(array, 'b');
  dyn_array_push(array, 'c');
  dyn_array_push(array, 'd');
  dyn_array_push(array, 'e');
  dyn_array_push(array, 'f');
  dyn_array_push(array, 'g');
  dyn_array_push(array, 'h');
  dyn_array_push(array, 'i');
  dyn_array_push(array, 'j');
  dyn_array_push(array, 'k');

  printf("size: %d\n", dyn_array_len(array));
  for (int i = 0; i < dyn_array_len(array); i++) {
    printf("array[%d]: %c\n", i, array[i]);
  }

  dyn_array_pop(array);
  dyn_array_pop(array);
  dyn_array_pop(array);
  dyn_array_pop(array);

  printf("second\n");
  for (int i = 0; i < dyn_array_len(array); i++) {
    printf("array[%d]: %c\n", i, array[i]);
  }


  return 0;
}
