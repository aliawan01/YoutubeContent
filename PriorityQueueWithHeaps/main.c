#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include "heap_print.h"

#define assert(condition) if (!(condition)) { fprintf(stderr, "Hit assertion line: %d, file: %s\n", __LINE__, __FILE__); __debugbreak(); }

void heapify(int* heap, int index, int count, bool max_or_min) {
  assert(index > 0);

  if (index >= count) {
    return;
  }

  if (index > (count/2)) {
    return;
  }

  int left_node = 2*index;
  int right_node = 2*index+1;

  int swap = index;

  if (left_node < count+1) {
    if (max_or_min) {
      if (heap[left_node-1] > heap[swap-1]) {
        swap = left_node;
      }
    }
    else {
      if (heap[left_node-1] < heap[swap-1]) {
        swap = left_node;
      }
    }
  }

  if (right_node < count+1) {
    if (max_or_min) {
      if (heap[right_node-1] > heap[swap-1]) {
        swap = right_node;
      }
    }
    else {
      if (heap[right_node-1] < heap[swap-1]) {
        swap = right_node;
      }
    }
  }

  if (swap != index) {
    int temp = heap[index-1];
    heap[index-1] = heap[swap-1];
    heap[swap-1] = temp;
    heapify(heap, swap, count, max_or_min);
  }
}

void heapify_bottom_up(int* heap, int count, bool max_or_min) {
  // NOTE(ali): max_or_min == True  -> Max-heap
  //            max_or_min == False -> Min-heap
  int last_parent_node_index = (count/2);
  for (int i = last_parent_node_index; i > 0; i--) {
    heapify(heap, i, count, max_or_min);
  }

}

void print_array(int* array, int count) {
  printf("%d", array[0]);
  for (int i = 1; i < count; i++) {
    printf(", %d", array[i]);
  }
  printf("\n");
}


void heapsort(int* heap, int count) {
  heapify_bottom_up(heap, count, true);

  for (int valid_count = count; valid_count > 1; valid_count--) {
    int temp = heap[0];
    heap[0] = heap[valid_count-1];
    heap[valid_count-1] = temp;

    heapify(heap, 1, valid_count-1, true);
  }
}

bool heapify_top_down(int* heap, int* index, int count, int new_element) {
  if (*index+1 > count) {
    return false;
  }

  memmove(heap+1, heap, sizeof(int)*(*index));
  heap[0] = new_element;
  ++*index;
  heapify(heap, 1, count, false);
  return true;
}

int priority_queue_dequeue(int* heap, int* index, int count) {
  if (*index <= 0) {
    return -1;
  }

  int elem = heap[0];
  memmove(heap, heap+1, sizeof(int*)*(*index)-1);
  heapify_bottom_up(heap, count, false);
  --*index;
  return elem;
}

int main(void) {
  int heap[20] = {};
  for (int i = 0; i < ArrayCount(heap); i++) {
    heap[i] = INT_MAX;
  }

  int index = 0;
  heapify_top_down(heap, &index, ArrayCount(heap), 10);
  heapify_top_down(heap, &index, ArrayCount(heap), 20);
  heapify_top_down(heap, &index, ArrayCount(heap), 5);
  heapify_top_down(heap, &index, ArrayCount(heap), 55);
  heapify_top_down(heap, &index, ArrayCount(heap), 1);
  heapify_top_down(heap, &index, ArrayCount(heap), 3);

  print_array(heap, index);
  int element_returned = priority_queue_dequeue(heap, &index, ArrayCount(heap));
  printf("-------------------------\nElement Returned: %d\n", element_returned);
  print_array(heap, index);

  element_returned = priority_queue_dequeue(heap, &index, ArrayCount(heap));
  printf("Element Returned: %d\n", element_returned);
  print_array(heap, index);

  element_returned = priority_queue_dequeue(heap, &index, ArrayCount(heap));
  printf("Element Returned: %d\n", element_returned);
  print_array(heap, index);

  element_returned = priority_queue_dequeue(heap, &index, ArrayCount(heap));
  printf("Element Returned: %d\n", element_returned);
  print_array(heap, index);

  element_returned = priority_queue_dequeue(heap, &index, ArrayCount(heap));
  printf("Element Returned: %d\n", element_returned);
  print_array(heap, index);
  element_returned = priority_queue_dequeue(heap, &index, ArrayCount(heap));
  printf("Element Returned: %d\n", element_returned);
  print_array(heap, index);
  element_returned = priority_queue_dequeue(heap, &index, ArrayCount(heap));
  printf("Element Returned: %d\n", element_returned);
  print_array(heap, index);
  return 0;
}
