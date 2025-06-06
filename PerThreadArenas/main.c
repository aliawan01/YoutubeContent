#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define KB(x) x*1024
#define GB(x) x*1024*1024
#define ArrayCount(x) sizeof(x)/sizeof(x[0])

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
    uint32_t id;
    Arena* scratchPool[2];
    Arena* globalArena;
} ThreadContext;


_Thread_local static ThreadContext ctx;

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

#define GetScratch(conflictingArenas, num) ScratchGetFree(ctx.scratchPool, 2, conflictingArenas, num)

DWORD WINAPI ThreadFunction(void* data) {
    ThreadContext ctx = *(ThreadContext*)data;

    if (ctx.id == 1) {
        char* string = ArenaAlloc(ctx.globalArena, 100, sizeof(char), _Alignof(char));
        strcpy(string, "hello world");
    }
    else {
        char* string = ArenaAlloc(ctx.globalArena, 100, sizeof(char), _Alignof(char));
        strcpy(string, "another string");
    }

    return 0;
}

int main(void) {
    Arena arena = {};
    ArenaInit(&arena, GB(10));

    HANDLE threadArray[5] = {0};
    ThreadContext threadContextArray[5] = {0};

    for (int i = 0; i < ArrayCount(threadArray); i++) {
        threadContextArray[i] = (ThreadContext) {
            .id = i+1,
            .globalArena = malloc(sizeof(Arena)),
            .scratchPool[0] = malloc(sizeof(Arena)),
            .scratchPool[1] = malloc(sizeof(Arena))
        };

        ArenaInit(threadContextArray[i].globalArena, KB(64));
        ArenaInit(threadContextArray[i].scratchPool[0], KB(64));
        ArenaInit(threadContextArray[i].scratchPool[1], KB(64));

        threadArray[i] = CreateThread(NULL, 0, ThreadFunction, (void*)&threadContextArray[i], 0, NULL);
    }

    

    ArenaDelete(&arena);

    return 0;
}
