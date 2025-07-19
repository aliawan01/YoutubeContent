#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <sanitizer/asan_interface.h>

#define ArrayCount(x) sizeof(x)/sizeof(x[0])
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
#ifdef DEBUG
  ASAN_POISON_MEMORY_REGION(arena->buffer, arena->currentOffset);
#else
    arena->currentOffset = 0;
#endif
}

void ArenaDelete(Arena* arena) {
    arena->currentOffset = 0;
    VirtualFree(arena->buffer, arena->currentOffset, MEM_RELEASE);
}


Temp TempBegin(Arena* arena) {
    return (Temp) { arena, arena->currentOffset };
}

void TempEnd(Temp temp) {
    temp.arena->currentOffset = temp.originalOffset;
}

Temp ScratchGetFree(Arena** arenaPool, int arenaPoolSize, Arena** conflictingArenas, int conflictingNum) {
    for (int i = 0; i < arenaPoolSize; i++) {
        bool isConflictingArena = false;
        for (int z = 0; z < conflictingNum; z++) {
            if (arenaPool[i] == conflictingArenas[z]) {
                isConflictingArena = true;
                break;
            }
        }

        if (!isConflictingArena) {
            return TempBegin(arenaPool[i]);
        }
    }

    return (Temp) { NULL, -1 };
}

void DeleteScratch(Temp tempScratch) {
    TempEnd(tempScratch);
}

#define PushArrayAlign(arena, type, num, align) (type*)ArenaAlloc(arena, (num), sizeof(type), align)
#define PushArray(arena, type, num) (type*)ArenaAlloc(arena, (num), sizeof(type), _Alignof(type))
#define PushString(arena, num) (char*)ArenaAlloc(arena, (num), sizeof(char), _Alignof(char))
#define PushStruct(arena, type) (type*)ArenaAlloc(arena, 1, sizeof(type), _Alignof(type))

int main(void) {
  Arena arena = {};
  ArenaInit(&arena, GB(10));

  char* a = ArenaAlloc(&arena, 20, sizeof(char), _Alignof(char));
  strcpy(a, "a string.");
  printf("%s\n", a);

  ArenaResetPointer(&arena);

  int* b = ArenaAlloc(&arena, 1, sizeof(int), _Alignof(int));
  char* other = ArenaAlloc(&arena, 20, sizeof(char), _Alignof(char));
  strcpy(other, "other thing");
  *b = 200;
  printf("other: `%s`, b: %d\n", other, *b);
  printf("a: `%s`\n", a);

  return 0;
}
