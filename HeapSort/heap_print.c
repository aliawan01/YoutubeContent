#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include "heap_print.h"

typedef struct {
  char string[512];
  int index;
} String;

#define ADD_X_SPACES(S, X) \
  for (int __i = 0; __i < X; __i++) { \
    S.string[S.index] = ' '; \
    S.index++; \
  }

void print_heap(int* heap, int count) {
  int num_of_layers = 1;
  for (int i = 0; i < count; i += pow(2, num_of_layers), num_of_layers += 1);

  String heap_string = {0};

  int heap_layers_array[10][30] = {0};
  int current_layer = 0;
  int current_index = 0;

  bool finished = false;
  while (!finished) {
    int num_of_elems_in_current_row = pow(2, current_layer);

    int i = 0;
    for (; i < num_of_elems_in_current_row; i++) {
      if (i+current_index >= count) {
        finished = true;
        break;
      }

      heap_layers_array[current_layer][i] = heap[i+current_index];
    }

    current_index += i;
    current_layer += 1;
  }

  for (int i = 0; i < num_of_layers; i++) {
    int* current_layer_elements = heap_layers_array[i];
    int num_of_elems_in_layer = pow(2, i);

    for (int z = 0; z < num_of_layers-i; z++) {
      ADD_X_SPACES(heap_string, 3);
    }

    if (i == 0) {
      ADD_X_SPACES(heap_string, 3);
    }
    else {
      ADD_X_SPACES(heap_string, i);
    }

    for (int z = 0; current_layer_elements[z] != 0 && z < num_of_elems_in_layer; z++) {
      if (num_of_elems_in_layer == 2 && z == 1) {
        ADD_X_SPACES(heap_string, 7);
      }
      else if (z > 0) {
        if (i%2 == 1 && z == i+1) {
          ADD_X_SPACES(heap_string, 1);
        }
        else if (i%2 == 0 && z == i) {
          ADD_X_SPACES(heap_string, 6);
        }
      }

      char character[8] = {};
      sprintf(character, "%d", current_layer_elements[z]);
      for (int x = 0; x < strlen(character); x++) {
        heap_string.string[heap_string.index] = character[x];
        heap_string.index++;
      }

      ADD_X_SPACES(heap_string, 1);
    }

    heap_string.string[heap_string.index] = '\n';
    heap_string.index++;
  }

  printf("%s\n", heap_string.string);
}
