#pragma once

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

void  ArenaInit(Arena* arena, uint64_t bufferSize); 
void* ArenaAlloc(Arena* arena, uint64_t numOfElem, uint64_t elemSize, uint64_t alignSize); 
void  ArenaResetPointer(Arena* arena); 
void  ArenaDelete(Arena* arena); 
Temp  TempBegin(Arena* arena); 
void  TempEnd(Temp temp);
Temp  ScratchGetFree(Arena** arenaPool, int arenaPoolSize, Arena** conflictingArenas, int conflictingNum); 
void  DeleteScratch(Temp tempScratch);

#define GetScratch(conflictingArenas, num) ScratchGetFree(ctx.scratchPool, 2, conflictingArenas, num)

#define PushArrayAlign(arena, type, num, align) (type*)ArenaAlloc(arena, (num), sizeof(type), align)
#define PushArray(arena, type, num) (type*)ArenaAlloc(arena, (num), sizeof(type), _Alignof(type))
#define PushString(arena, num) (char*)ArenaAlloc(arena, (num), sizeof(char), _Alignof(char))
#define PushStruct(arena, type) (type*)ArenaAlloc(arena, 1, sizeof(type), _Alignof(type))

#define KB(x) x*1024
#define GB(x) x*1024*1024
#define ArrayCount(x) sizeof(x)/sizeof(x[0])

