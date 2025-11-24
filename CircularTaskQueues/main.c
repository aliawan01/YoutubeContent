#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define QUEUE_MAX_SIZE 10
#define MAX_THREADS 10

typedef struct {
    int data[QUEUE_MAX_SIZE];
    int frontPointer;
    int backPointer;
    int size;
    int occupied;
} CircularQueue;

bool enqueue(CircularQueue* queue, int element, HANDLE semaphore, CRITICAL_SECTION* critical_section) {
    EnterCriticalSection(critical_section);

    bool success = false;
    if (queue->occupied < queue->size) {
        queue->data[queue->backPointer] = element;
        queue->backPointer = (queue->backPointer+1) % queue->size;
        queue->occupied++;
        ReleaseSemaphore(semaphore, 1, NULL);
        success = true;
    }

    LeaveCriticalSection(critical_section);
    return success;
}

int dequeue(CircularQueue* queue, CRITICAL_SECTION* critical_section) {
    EnterCriticalSection(critical_section);

    int element = -1;
    if (queue->occupied > 0) {
        element = queue->data[queue->frontPointer];
        queue->data[queue->frontPointer] = 0;
        queue->frontPointer = (queue->frontPointer+1) % queue->size;
        queue->occupied--;
    }

    LeaveCriticalSection(critical_section);
    return element;
}

int peek(CircularQueue* queue, CRITICAL_SECTION* critical_section) {
    EnterCriticalSection(critical_section);

    int element = -1;
    if (queue->occupied > 0) {
        element = queue->data[queue->frontPointer];
    }

    LeaveCriticalSection(critical_section);
    return -1;
}

typedef struct {
  int id;
  HANDLE* semaphore;
  CircularQueue* queue;
  CRITICAL_SECTION* critical_section;
} ThreadParams;

DWORD WINAPI ThreadFunc(void* params) {
  ThreadParams* thread_params = (ThreadParams*)params;

  while (true) {
    WaitForSingleObject(thread_params->semaphore, INFINITE);
    int element = dequeue(thread_params->queue, thread_params->critical_section);

    printf("Thread ID: %d, element: %d\n", thread_params->id, element);
  }

  return 0;
}

int main(void)  {
    CircularQueue queue = {
        .size = QUEUE_MAX_SIZE 
    };

    CRITICAL_SECTION critical_section;
    InitializeCriticalSection(&critical_section);

    HANDLE semaphore = CreateSemaphoreA(NULL, 0, QUEUE_MAX_SIZE, NULL);

    HANDLE thread_array[MAX_THREADS] = {0};
    ThreadParams thread_params_array[MAX_THREADS] = {0};

    for (int i = 0; i < MAX_THREADS; i++) {
      thread_params_array[i] = (ThreadParams) {
        .id = i,
        .semaphore = semaphore,
        .queue = &queue,
        .critical_section = &critical_section
      };

      thread_array[i] = CreateThread(NULL, 0, ThreadFunc, &thread_params_array[i], 0, NULL);
    }

    enqueue(&queue, 10, semaphore, &critical_section);
    enqueue(&queue, 20, semaphore, &critical_section);
    enqueue(&queue, 30, semaphore, &critical_section);
    enqueue(&queue, 40, semaphore, &critical_section);
    enqueue(&queue, 50, semaphore, &critical_section);
    enqueue(&queue, 23, semaphore, &critical_section);
    enqueue(&queue, 46, semaphore, &critical_section);
    enqueue(&queue, 812, semaphore, &critical_section);


    WaitForMultipleObjects(MAX_THREADS, thread_array, TRUE, INFINITE);

    return 0;
}
