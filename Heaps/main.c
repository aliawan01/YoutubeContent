#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include "heap_print.h"

void heapify(int* heap, int index, int count, bool max_or_min) {
  if (index > count || index < 1) {
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

int main(void) {
  //                          1  2   3   4   5  6   7  8   9   10
  int unordered_array[10] = { 5, 10, 15, 98, 4, 13, 5, 61, 49, 2 };
  printf("BEFORE:\n");
  print_heap(unordered_array, ArrayCount(unordered_array));
  heapify_bottom_up(unordered_array, ArrayCount(unordered_array), false);
  printf("===========================\nAFTER:\n");
  print_heap(unordered_array, ArrayCount(unordered_array));

  return 0;
}
