#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define __DEFER__(func_name, var_name) \
    auto void func_name(int*); \
    int var_name __attribute__((__cleanup__(func_name))); \
    auto void func_name(int*) 

#define DEFER_ONE(N) __DEFER__(__DEFER__FUNC ## N, __DEFER__VAR ## N)
#define DEFER_TWO(N) DEFER_ONE(N)
#define defer DEFER_TWO(__COUNTER__)

int main(void) {
    char* string_on_heap = malloc(1000);
    defer {free(string_on_heap);};
    defer { printf("some string"); };
    {
        strcpy(string_on_heap, "hello world\n");
        printf("%s\n", string_on_heap);
    }



    return 0;
}
