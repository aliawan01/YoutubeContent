#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define QUEUE_MAX_SIZE 10

typedef struct {
    int data[QUEUE_MAX_SIZE];
    int frontPointer;
    int backPointer;
    int size;
    int occupied;
} Queue;

bool enqueue(Queue* queue, int element) {
    if (queue->occupied < queue->size) {
        queue->data[queue->backPointer] = element;
        queue->backPointer = (queue->backPointer+1) % queue->size;
        queue->occupied++;
        return true;
    }

    // NOTE(ali): Overflow
    return false;
}

int dequeue(Queue* queue) {
    if (queue->occupied > 0) {
        int element = queue->data[queue->frontPointer];
        queue->data[queue->frontPointer] = 0;
        queue->frontPointer = (queue->frontPointer+1) % queue->size;
        queue->occupied--;
        return element;
    }

    // NOTE(ali): Underflow
    return -1;
}

int peek(Queue queue) {
    if (queue.occupied > 0) {
        return queue.data[queue.frontPointer];
    }

    return -1;
}

int main(void)  {
    Queue queue = {
        .size = QUEUE_MAX_SIZE 
    };

    printf("Peek: `%d`\n", peek(queue));
    enqueue(&queue, 20);
    enqueue(&queue, 25);
    enqueue(&queue, 30);
    printf("Peek: `%d`\n", peek(queue));
    dequeue(&queue);
    printf("Peek: `%d`\n", peek(queue));
    dequeue(&queue);
    printf("Peek: `%d`\n", peek(queue));
    dequeue(&queue);
    printf("Peek: `%d`\n", peek(queue));
    dequeue(&queue);
    printf("Peek: `%d`\n", peek(queue));



    return 0;
}
