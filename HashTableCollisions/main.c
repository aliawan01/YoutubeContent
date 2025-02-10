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
} HashTable;

void HashTableInitialize(HashTable* hashTable, int count) {
    *hashTable = (HashTable) {
        .data = malloc(sizeof(HashNode)*count),
        .count = count
    };

    memset(hashTable->data, 0, sizeof(HashNode)*count);
}


unsigned long GenHash(char *str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;

    return hash;
}

bool HashTableAdd(HashTable* hashTable, char* key, char* value) {
    int index = GenHash(key) % hashTable->count;

    if (hashTable->data[index].key == NULL) {
        hashTable->data[index] = (HashNode) {
            .key = strdup(key),
            .value = strdup(value),
            .parent = NULL,
            .child = NULL
        };

        return true;
    }
    else {
        HashNode* node = &hashTable->data[index];
        for (; node->child != NULL; node = node->child);
        node->child = malloc(sizeof(HashNode));
        *node->child = (HashNode) {
            .key = strdup(key),
            .value = strdup(value),
            .parent = node,
            .child = NULL
        };

        return true;
    }


    return false;
}

char* HashTableGetValueAtKey(HashTable* hashTable, char* key) {
    int index = GenHash(key) % hashTable->count;

    if (hashTable->data[index].key != NULL) {
        for (HashNode* node = &hashTable->data[index]; node != NULL; node = node->child) {
            if (!strcmp(node->key, key)) {
                return node->value;
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
        if (!strcmp(node->key, key)) {
            if (node->parent == NULL) {
                free(node->key);
                free(node->value);

                if (node->child != NULL) {
                    hashTable->data[index] = *node->child;
                    node->child->parent = NULL;
                }

                return true;
            }
            else if (node->parent != NULL && node->child != NULL) {
                free(node->key);
                free(node->value);
                node->parent->child = node->child;
                free(node);
                return true;
            }
            else {
                node->parent = NULL;
                free(node->key);
                free(node->value);
                free(node);
                return true;
            }
        }
    }

    return false;
}

int main(void) {
    HashTable hashTable = {};
    HashTableInitialize(&hashTable, 3);

    int success = HashTableAdd(&hashTable, "name", "Ali");
    printf("Success adding key: %d\n", success);
    success = HashTableAdd(&hashTable, "surname", "Awan");
    success = HashTableAdd(&hashTable, "ala", "Ali");
    printf("Success adding key: %d\n", success);
    success = HashTableAdd(&hashTable, "afa", "Awan");
    printf("Success adding key: %d\n", success);
    success = HashTableAdd(&hashTable, "af", "Awan");
    printf("Success adding key: %d\n", success);
    success = HashTableAdd(&hashTable, "fa", "Awan");
    printf("Key: `%s`, Value: `%s`\n", "surname", HashTableGetValueAtKey(&hashTable, "surname"));
    printf("Key: `%s`, Value: `%s`\n", "asdlkfjsd;l", HashTableGetValueAtKey(&hashTable, "asdlkfjsd;l"));
    printf("Removed key: %d\n", HashTableRemove(&hashTable, "surname"));
    printf("Removed key: %d\n", HashTableRemove(&hashTable, "random"));


    return 0;
}
