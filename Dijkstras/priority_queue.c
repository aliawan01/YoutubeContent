#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "priority_queue.h"

void PriorityQueueInitialize(PriorityQueue* queue, int size) {
    *queue = (PriorityQueue) {
        .data = calloc(size, sizeof(Vec2AndCost)),
        .size = size
    };
}

void PriorityQueueDelete(PriorityQueue* queue) {
    free(queue->data);
}

int PriorityQueueGetCostOfElement(PriorityQueue queue, Vector2 element) {
    for (int i = 0; i < queue.occupied; i++) {
        if (queue.data[i].vec2.y == element.y && queue.data[i].vec2.x == element.x) {
            return queue.data[i].cost;
        }
    }

    return -1;
}

bool Enqueue(PriorityQueue* queue, Vector2 element, int cost) {
    Vec2AndCost data = {element, cost};
    if (queue->occupied < queue->size) {
        queue->data[queue->occupied] = data;
        queue->occupied++;
        return true;
    }

    // NOTE(ali): Overflow
    return false;
}

bool PriorityQueueReplaceOrAddIfNotExist(PriorityQueue* queue, Vector2 element, int cost) {
    for (int i = 0; i < queue->occupied; i++) {
        if (queue->data[i].vec2.x == element.x && queue->data[i].vec2.y == element.y) {
            queue->data[i].cost = cost;
            return true;
        }
    }

    return Enqueue(queue, element, cost);
}

Vec2AndCost Dequeue(PriorityQueue* queue) {
    if (queue->occupied > 0) {
        int lowestCostIndex = 0;
        for (int i = 0; i < queue->occupied; i++) {
            if (queue->data[i].cost < queue->data[lowestCostIndex].cost) {
                lowestCostIndex = i;
            }
        }

        Vec2AndCost element = queue->data[lowestCostIndex];
        memmove(queue->data+lowestCostIndex, queue->data+lowestCostIndex+1, (queue->occupied-lowestCostIndex)*sizeof(Vec2AndCost));
        queue->occupied--;

        return element;
    }

    // NOTE(ali): Underflow
    return (Vec2AndCost){INVALID_VEC2, -1};
}

Vec2AndCost Peek(PriorityQueue queue) {
    if (queue.occupied > 0) {
        return queue.data[queue.occupied-1];
    }

    return (Vec2AndCost){INVALID_VEC2, -1};
}

bool PriorityQueueIsEmpty(PriorityQueue queue) {
    return queue.occupied > 0 ? false : true; 
}

bool PriorityQueueElementExists(PriorityQueue queue, Vector2 element) {
    for (int i = 0; i < queue.occupied; i++) {
        if (queue.data[i].vec2.x == element.x && queue.data[i].vec2.y == element.y) {
            return true;
        }
    }

    return false;
}
