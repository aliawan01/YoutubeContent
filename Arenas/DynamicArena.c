#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/*
This is done freehand, not guarenteed to work one first run.  This is sample content for you.
  */

typedef struct {
    bool dispose;  // Flag to mark for deletion
    char *someData;
} Node;

typedef struct {
    Node *nodes;
    size_t capacity;
    size_t used;
} Arena;

// Create an arena with an initial capacity
Arena create_arena(size_t initial_capacity) {
    Arena arena;
    arena.nodes = (Node *)calloc(initial_capacity, sizeof(Node));
    arena.capacity = initial_capacity;
    arena.used = 0;
    return arena;
}

// Free all deleted nodes
void free_deleted_nodes(Arena *arena) {
    size_t new_used = 0;
    for (size_t i = 0; i < arena->used; i++) {
        if (arena->nodes[i].dispose) {
            free(arena->nodes[i].someData);  // Free data
        } else {
            arena->nodes[new_used++] = arena->nodes[i];  // Keep valid nodes
        }
    }
    arena->used = new_used; // Update count after compaction
}

// Resize arena when full (after trying to free deleted nodes first)
void resize_arena(Arena *arena) {
    free_deleted_nodes(arena);

    if (arena->used < arena->capacity) {
        return;  // No need to resize if space was reclaimed
    }

    size_t new_capacity = arena->capacity * 2;
    arena->nodes = (Node *)realloc(arena->nodes, new_capacity * sizeof(Node));
    if (!arena->nodes) {
        fprintf(stderr, "Failed to allocate more memory!\n");
        exit(1);
    }
    memset(&arena->nodes[arena->capacity], 0, (new_capacity - arena->capacity) * sizeof(Node));
    arena->capacity = new_capacity;
}

// Allocate a new node in the arena
Node *allocate_node(Arena *arena, const char *data) {
    resize_arena(arena);

    if (arena->used >= arena->capacity) {
        return NULL; // Should not happen due to resizing
    }

    Node *node = &arena->nodes[arena->used++];
    node->dispose = false;
    node->someData = strdup(data);
    return node;
}

// Free the arena and all nodes
void destroy_arena(Arena *arena) {
    for (size_t i = 0; i < arena->used; i++) {
        free(arena->nodes[i].someData);
    }
    free(arena->nodes);
    arena->nodes = NULL;
    arena->capacity = 0;
    arena->used = 0;
}

// Test the dynamic arena
int main() {
    Arena arena = create_arena(2);

    Node *n1 = allocate_node(&arena, "Hello");
    Node *n2 = allocate_node(&arena, "World");

    printf("Before marking delete: %s, %s\n", n1->someData, n2->someData);

    n1->dispose = true; // Mark n1 for deletion

    allocate_node(&arena, "NewData"); // Should reuse n1's space if freed

    printf("After cleanup:\n");
    for (size_t i = 0; i < arena.used; i++) {
        printf("Node[%zu]: %s\n", i, arena.nodes[i].someData);
    }

    destroy_arena(&arena);
    return 0;
}
