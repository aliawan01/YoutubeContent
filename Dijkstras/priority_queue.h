#pragma once

#define INVALID_VEC2 (Vector2){-1, -1}

typedef struct {
    int x;
    int y;
} Vector2;

typedef struct {
    Vector2 vec2;
    int cost;
} Vec2AndCost; 

typedef struct {
    Vec2AndCost* data;
    int* cost;
    int size;
    int occupied;
} PriorityQueue;

void    PriorityQueueInitialize(PriorityQueue* queue, int size);
void    PriorityQueueDelete(PriorityQueue* queue);
int     PriorityQueueGetCostOfElement(PriorityQueue queue, Vector2 element);
bool    PriorityQueueIsEmpty(PriorityQueue queue);
bool    PriorityQueueElementExists(PriorityQueue queue, Vector2 element);
bool    Enqueue(PriorityQueue* queue, Vector2 element, int cost);
bool    PriorityQueueReplaceOrAddIfNotExist(PriorityQueue* queue, Vector2 element, int cost);
Vec2AndCost Dequeue(PriorityQueue* queue);
Vec2AndCost Peek(PriorityQueue queue);
