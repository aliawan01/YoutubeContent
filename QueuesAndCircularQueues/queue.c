#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define QUEUE_MAX_SIZE 10
typedef struct {
    int data[QUEUE_MAX_SIZE];
    int size;
    int frontPointer;
    int rearPointer;
} Queue;

bool enqueue(Queue* queue, int element) {
    if (queue->size < QUEUE_MAX_SIZE) {
        queue->data[queue->rearPointer] = element;
        queue->rearPointer++;
        queue->size++;
        return true;
    }

    return false;
}

int dequeue(Queue* queue) {
    if (queue->size > 0) {
        int element = queue->data[queue->frontPointer];
        queue->data[queue->frontPointer] = 0;
        queue->frontPointer++;
        queue->size--;
        return element;
    }

    return -1;
}

int peek(Queue queue) {
    if (queue.size > 0) {
        return queue.data[queue.frontPointer];
    }

    return -1;
}

int main(void) {
    Queue queue = {0};

    enqueue(&queue, 10);
    printf("Peek: %d\n", peek(queue));
    enqueue(&queue, 20);
    printf("Peek: %d\n", peek(queue));
    enqueue(&queue, 30);
    printf("Peek: %d\n", peek(queue));
    dequeue(&queue);
    printf("Peek: %d\n", peek(queue));
    dequeue(&queue);
    printf("Peek: %d\n", peek(queue));
    dequeue(&queue);
    printf("Peek: %d\n", peek(queue));
    dequeue(&queue);
    printf("Peek: %d\n", peek(queue));

    return 0;
}
