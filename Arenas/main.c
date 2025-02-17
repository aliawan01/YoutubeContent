#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define KB(x) x*1024

typedef struct {
    char* buffer;
    uint64_t currentOffset;
    uint64_t bufferSize;
} Arena;

void ArenaInit(Arena* arena, uint64_t bufferSize) {
    *arena = (Arena) {
        .buffer = malloc(bufferSize),
        .bufferSize = bufferSize,
        .currentOffset = 0
    };
}

void* ArenaAlloc(Arena* arena, uint64_t numOfElem, uint64_t elemSize, uint64_t alignSize) {
    if (alignSize == 0 || (alignSize & (alignSize-1)) == 1) {
        return NULL;
    }

    uintptr_t allocationSize = numOfElem*elemSize;
     
    if (allocationSize < elemSize) {
        return NULL;
    }

    uintptr_t totalOffset = (uintptr_t)arena->currentOffset + (uintptr_t)arena->buffer;
    uintptr_t padding = (~totalOffset+1) & (alignSize-1);

    totalOffset += padding;
    if (totalOffset + allocationSize > (uintptr_t)arena->bufferSize + (uintptr_t)arena->buffer) {
        return NULL;
    }

    arena->currentOffset += (padding + allocationSize);
    memset((void*)totalOffset, 0, allocationSize);
    return (void*)totalOffset;
}

void ArenaResetPointer(Arena* arena) {
    arena->currentOffset = 0;
    memset(arena->buffer, 0, arena->bufferSize);
}

void ArenaDelete(Arena* arena) {
    arena->currentOffset = 0;
    free(arena->buffer);
}

int main(void) {
    Arena arena = {};
    ArenaInit(&arena, KB(10));

    char* a = ArenaAlloc(&arena, 100, sizeof(char), _Alignof(char));
    strcpy(a, "a string.");
    printf("`%s`\n", a);

    ArenaDelete(&arena);

    return 0;
}
