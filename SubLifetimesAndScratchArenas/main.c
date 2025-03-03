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

typedef struct {
    Arena* arena;
    uint64_t originalOffset;
} Temp;

typedef struct {
    Arena* scratchPool[2];
    Arena* globalArena;
} Context;

static Context ctx;

Temp TempBegin(Arena* arena) {
    return (Temp) { arena, arena->currentOffset };
}

void TempEnd(Temp temp) {
    temp.arena->currentOffset = temp.originalOffset;
}

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

Temp ScratchGetFree(Arena** arenaPool, int arenaPoolSize, Arena** conflictingArenas, int conflictingArenaSize) {
    for (int i = 0; i < arenaPoolSize; i++) {
        bool isConflictingArena = false;
        for (int z = 0; z < conflictingArenaSize; z++) {
            if (arenaPool[i] == conflictingArenas[z]) {
                isConflictingArena = true;
            }
        }

        if (!isConflictingArena) {
            return TempBegin(arenaPool[i]);
        }
    }

    return (Temp) {NULL, -1};
}

void ScratchFree(Temp scratch) {
    TempEnd(scratch);
}

#define GetScratch(conflictingArenas, num) ScratchGetFree(ctx.scratchPool, 2, conflictingArenas, num)

void foo(Arena* something) {
    Temp scratch = GetScratch(&something, 1);
}

void something(Arena* something) {
    Temp scratch = GetScratch(0, 0);
    foo(scratch.arena);
}

int main(void) {
    ctx.globalArena = malloc(sizeof(Arena));
    ctx.scratchPool[0] = malloc(sizeof(Arena));
    ctx.scratchPool[0] = malloc(sizeof(Arena));

    ArenaInit(ctx.scratchPool[0], GB(1));
    ArenaInit(ctx.scratchPool[1], GB(1));
    ArenaInit(ctx.globalArena, GB(1));

    Temp scratch = GetScratch(0, 0);
    /* Arena arena = {}; */
    /* ArenaInit(&arena, GB(10)); */
    /* char* stringCopy = ArenaAlloc(&arena, 100, sizeof(char), _Alignof(char)); */

    /* int* array = ArenaAlloc(temp.arena, 1000, sizeof(int), _Alignof(int)); */
    /* array[599] = 600; */
    /* char* a = ArenaAlloc(temp.arena, 100, sizeof(char), _Alignof(char)); */
    /* strcpy(a, "a string."); */
    /* printf("`%s`, %d\n", a, array[599]); */

    /* /1* ArenaDelete(&arena); *1/ */
    /* TempEnd(temp); */
    /* printf("Arena Offset: %lld\n", arena.currentOffset); */


    return 0;
}
