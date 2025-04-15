#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef struct {
  void (*defer_func)(void*);
  void* value;
} DeferFunc;

typedef struct {
  DeferFunc funcs[50];
  uint64_t count;
} DeferFuncArray;


#define DEFER_START for (DeferFuncArray defer_func_array = {};;) {
#define DEFER_END \
    for (int i = 0; i < defer_func_array.count; i++) { \
      DeferFunc current_defer_func = defer_func_array.funcs[i]; \
      current_defer_func.defer_func(current_defer_func.value); \
    } \
    break; \
  }

#define DEFER(DEFER_FUNC_NAME, DEFER_VALUE) \
  defer_func_array.funcs[defer_func_array.count].defer_func = DEFER_FUNC_NAME; \
  defer_func_array.funcs[defer_func_array.count].value = DEFER_VALUE; \
  defer_func_array.count++;


void print_string(char* string) {
  printf("print_string: `%s`\n", string);
}

void free_string(char* string) {
  printf("Freed string!\n");
  free(string);
}

int main(void) {
  DEFER_START
  char* string_on_heap = malloc(1000);
  DEFER(free_string, string_on_heap);

  strcpy(string_on_heap, "this is a string");
  printf("string_on_heap: `%s`\n", string_on_heap);


  DEFER(print_string, "other_string");

  DEFER_END

  return 0;
}
