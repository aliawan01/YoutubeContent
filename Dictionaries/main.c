#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    char** keys;
    char** values;
    int size;
    int occupied;
} Dict;

void DictIntialize(Dict* dict, int size, int maxStringSize) {
    *dict = (Dict) {
        .keys = malloc(sizeof(char*)*size),
        .values = malloc(sizeof(char*)*size),
        .size = size
    };

    for (int i = 0; i < size; i++) {
        dict->keys[i] = malloc(maxStringSize);
        dict->values[i] = malloc(maxStringSize);
    }
}

bool DictAdd(Dict* dict, char* key, char* value) {
    for (int i = 0; i < dict->occupied; i++) {
        if (!strcmp(dict->keys[i], key)) {
            return false;
        }
    }

    if (dict->occupied < dict->size) {
        strcpy(dict->keys[dict->occupied], key);
        strcpy(dict->values[dict->occupied], value);
        dict->occupied++;
        return true;
    }

    return false;
}

char* DictGetValueAtKey(Dict* dict, char* key) {
    for (int i = 0; i < dict->occupied; i++) {
        if (!strcmp(dict->keys[i], key)) {
            return dict->values[i];
        }
    }

    return NULL;
}

bool DictRemoveKeyValuePair(Dict* dict, char* key) {
    for (int i = 0; i < dict->occupied; i++) {
        if (!strcmp(dict->keys[i], key)) {
            memmove(dict->keys+i, dict->keys+i+1, dict->occupied-i);
            memmove(dict->values+i, dict->values+i+1, dict->occupied-i);
            return true;
        }
    }

    return false;
}

int main(void) {
    Dict dict = {};
    DictIntialize(&dict, 5, 50);

    printf("got here.\n");
    printf("Success: `%d`\n", DictAdd(&dict, "1", "world!"));
    printf("Success: `%d`\n", DictAdd(&dict, "2", "world!"));
    printf("Success: `%d`\n", DictAdd(&dict, "3", "world!"));
    printf("Success: `%d`\n", DictAdd(&dict, "4", "world!"));
    printf("value: `%s`\n", DictGetValueAtKey(&dict, "4"));
    printf("successfully removed 4: `%d`\n", DictRemoveKeyValuePair(&dict, "4"));
    printf("value: `%s`\n", DictGetValueAtKey(&dict, "4"));
    printf("successfully removed 4: `%d`\n", DictRemoveKeyValuePair(&dict, "4"));

    return 0;
}
