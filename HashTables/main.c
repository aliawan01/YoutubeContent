#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

typedef struct {
    char** keys;
    char** values;
    int count;
} HashTable;

void HashTableInitialize(HashTable* hashTable, int count) {
    *hashTable = (HashTable) {
        .keys = malloc(sizeof(char*)*count),
        .values = malloc(sizeof(char*)*count),
        .count = count
    };

    memset(hashTable->keys, 0, sizeof(char*)*count);
    memset(hashTable->values, 0, sizeof(char*)*count);
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

    if (hashTable->keys[index] != NULL) {
        printf("[COLLISION!!!] Key exists at index: %d, `%s`.\n", index, hashTable->keys[index]);
        return false;
    }

    hashTable->keys[index] = strdup(key);
    hashTable->values[index] = strdup(value);

    return true;
}

char* HashTableGetValueAtKey(HashTable* hashTable, char* key) {
    int index = GenHash(key) % hashTable->count;

    if (hashTable->keys[index] != NULL) {
        return hashTable->values[index];
    }
    else {
        return NULL;
    }
}

bool HashTableRemove(HashTable* hashTable, char* key) {
    int index = GenHash(key) % hashTable->count;


    if (hashTable->keys[index] == NULL) {
        return false;
    }
    else if (strcmp(hashTable->keys[index], key) != 0) {
        return false;
    }

    free(hashTable->keys[index]);
    free(hashTable->values[index]);

    memset(hashTable->keys+index, 0, sizeof(char*));
    memset(hashTable->values+index, 0, sizeof(char*));

    return true;
}

int main(void) {
    HashTable hashTable = {};
    HashTableInitialize(&hashTable, 10);

    int success = HashTableAdd(&hashTable, "name", "Ali");
    printf("Success adding key: %d\n", success);
    success = HashTableAdd(&hashTable, "surname", "Awan");
    printf("Success adding key: %d\n", success);
    printf("Key: `%s`, Value: `%s`\n", "surname", HashTableGetValueAtKey(&hashTable, "surname"));
    printf("Key: `%s`, Value: `%s`\n", "asdlkfjsd;l", HashTableGetValueAtKey(&hashTable, "asdlkfjsd;l"));
    printf("Removed key: %d\n", HashTableRemove(&hashTable, "surname"));
    printf("Removed key: %d\n", HashTableRemove(&hashTable, "random"));
    success = HashTableAdd(&hashTable, "al", "Awan");
    printf("Success adding key: %d\n", success);
    success = HashTableAdd(&hashTable, "alb", "Awan");
    printf("Success adding key: %d\n", success);
    success = HashTableAdd(&hashTable, "ssdflkjsdafurname", "Awan");
    printf("Success adding key: %d\n", success);
    success = HashTableAdd(&hashTable, "flkj;urname", "Awan");
    printf("Success adding key: %d\n", success);
    success = HashTableAdd(&hashTable, "askdfsurname", "Awan");
    printf("Success adding key: %d\n", success);


    return 0;
}
