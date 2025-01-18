#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    void* (*alloc)(size_t size);
    void  (*dealloc)(void* pointer);
} DictAllocHooks;

typedef struct {
    char** keys;
    char** values;
    int size;
    int occupied;
    DictAllocHooks hooks;
} Dict;

void DictIntialize(Dict* dict, int size, int maxStringSize, DictAllocHooks hooks) {
    dict->hooks  = hooks;
    dict->keys   = hooks.alloc(sizeof(char*)*size);
    dict->values = hooks.alloc(sizeof(char*)*size);
    dict->size   = size;
    dict->hooks  = hooks;
}

bool DictAdd(Dict* dict, char* key, char* value) {
    for (int i = 0; i < dict->occupied; i++) {
        if (!strcmp(dict->keys[i], key)) {
            return false;
        }
    }

    if (dict->occupied < dict->size) {
        dict->keys[dict->occupied] = dict->hooks.alloc(strlen(key)+1);
        dict->values[dict->occupied] = dict->hooks.alloc(strlen(value)+1);

        memset(dict->keys[dict->occupied], 0, strlen(key)+1);
        memset(dict->values[dict->occupied], 0, strlen(key)+1);

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
            dict->hooks.dealloc(dict->keys[i]);
            dict->hooks.dealloc(dict->values[i]);

            memmove(dict->keys+i, dict->keys+i+1, dict->occupied-i);
            memmove(dict->values+i, dict->values+i+1, dict->occupied-i);
            return true;
        }
    }

    return false;
}

int main(void) {
    Dict dict = {};
    DictAllocHooks hooks = { malloc, free };
    DictIntialize(&dict, 5, 50, hooks);

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
