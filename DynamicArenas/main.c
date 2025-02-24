#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define KB(x) x*1024
#define GB(x) x*1024*1024

typedef struct {
    char* buffer;
    uint64_t currentOffset;
    uint64_t bufferSize;
    uint64_t pageSize;
    uint64_t numOfPagesAllocated;
} Arena;

void ArenaInit(Arena* arena, uint64_t bufferSize) {
    SYSTEM_INFO systemInfo = {};
    GetSystemInfo(&systemInfo);

    *arena = (Arena) {
        .buffer = VirtualAlloc(0, bufferSize, MEM_RESERVE, PAGE_READWRITE),
        .bufferSize = bufferSize,
        .currentOffset = 0,
        .pageSize = systemInfo.dwPageSize,
        .numOfPagesAllocated = 0
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

    double diviser = (arena->currentOffset+padding+allocationSize)/arena->pageSize+1;
    if (diviser > arena->numOfPagesAllocated) {
        uint64_t numOfNewPagesToAllocate = (uint64_t)diviser - arena->numOfPagesAllocated +1;
        VirtualAlloc((uint8_t*)arena->buffer+arena->currentOffset, numOfNewPagesToAllocate*arena->pageSize, MEM_COMMIT, PAGE_READWRITE);
    }

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
    VirtualFree(arena->buffer, arena->currentOffset, MEM_RELEASE);
}

int main(void) {
    Arena arena = {};
    ArenaInit(&arena, GB(10));

    int* array = ArenaAlloc(&arena, 1000, sizeof(int), _Alignof(int));
    array[599] = 600;
    char* a = ArenaAlloc(&arena, 100, sizeof(char), _Alignof(char));
    strcpy(a, "a string.");
    printf("`%s`, %d\n", a, array[599]);

    ArenaDelete(&arena);

    return 0;
}
