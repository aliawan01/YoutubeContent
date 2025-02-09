#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

typedef struct HashNode {
    char* key;
    char* value;
    struct HashNode* parent;
    struct HashNode* child;
} HashNode;

typedef struct {
    HashNode* data;
    int count;
    int occupied;
} HashTable;

void HashTableInit(HashTable* hashTable, int count) {
    *hashTable = (HashTable) {
        .data = malloc(sizeof(HashNode)*count),
        .count = count,
        .occupied = 0
    };

    memset(hashTable->data, 0, sizeof(HashNode)*count);
}

unsigned long GenHash(char* string) {
    unsigned long hash = 5381;
    int c;

    while ((c = *string++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

bool HashTableAdd(HashTable* hashTable, char* key, char* value) {
    int index = GenHash(key) % hashTable->count;

    HashNode* node = &hashTable->data[index];
    if (node->key == NULL) {
        *node = (HashNode) {
            .key = strdup(key),
            .value = strdup(value),
            .child = NULL,
            .parent = NULL
        };

        return true;
    }
    else {
        for (; node->child != NULL; node = node->child);
        node->child = malloc(sizeof(HashNode));
        *node->child = (HashNode) {
            .key = strdup(key),
            .value = strdup(value),
            .child = NULL,
            .parent = node 
        };

        return true;
    }

    return true;
}

char* HashTableGetValueAtKey(HashTable* hashTable, char* key) {
    int index = GenHash(key) % hashTable->count;

    if (hashTable->data[index].key != NULL) {
        for (HashNode* node = &hashTable->data[index]; node != NULL; node = node->child) {
            if (!strcmp(node->key, key)) {
                return hashTable->data[index].value;
            }
        }
    }

    return NULL;
}

bool HashTableRemove(HashTable* hashTable, char* key) {
    int index = GenHash(key) % hashTable->count;

    if (hashTable->data[index].key == NULL) {
        return false;
    }

    for (HashNode* node = &hashTable->data[index]; node != NULL; node = node->child) {
        if (node->parent == NULL) {
            if (!strcmp(node->key, key)) {
                free(node->key);
                free(node->value);
                node->key = NULL;
                node->value = NULL;
            }
        }
        else if (node->child != NULL) {
            if (!strcmp(node->key, key)) {
                free(node->key);
                free(node->value);
                node->child->parent = node->parent;
                free(node);
            }
        }
        else if (node->child == NULL) {
            if (!strcmp(node->key, key)) {
                node = node->parent;
                free(node->child->key);
                free(node->child->value);
                free(node->child);
                node->child = NULL;
            }
        }
    }

    return true;
}

int main(void) {
    HashTable hashTable = {};
    HashTableInit(&hashTable, 3);

    int success = HashTableAdd(&hashTable, "good", "morning");
    printf("Success: %d\n", success);
    success = HashTableAdd(&hashTable, "some", "else");
    printf("Success: %d\n", success);
    success = HashTableAdd(&hashTable, "aoosl", "else");
    printf("Success: %d\n", success);
    success = HashTableAdd(&hashTable, "'ffff", "else");
    printf("Success: %d\n", success);
    success = HashTableAdd(&hashTable, "fdsaf", "else");
    printf("Success: %d\n", success);
    success = HashTableAdd(&hashTable, "dfdas", "else");
    printf("Success: %d\n", success);
    success = HashTableAdd(&hashTable, "alsd", "else");
    printf("Success: %d\n", success);

    printf("key: `dfdas` value: `%s`\n", HashTableGetValueAtKey(&hashTable, "dfdas"));
    printf("Removed key: `dfdas` successfully `%d`\n", HashTableRemove(&hashTable, "dfdas"));
    printf("key: `dfdas` value: `%s`\n", HashTableGetValueAtKey(&hashTable, "dfdas"));

    return 0;
}
