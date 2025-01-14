#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>


#define FindType(value) _Generic((value),\
        char*:   (DictValue) { .string = #value }, \
        int:     (DictValue) { .integer = value }, \
        double:  (DictValue) { .doubleVal = value }, \
        float:   (DictValue) { .floatVal = value }, \
        default: (DictValue) { .integer = 0 } \
)

#define DictAdd(dict, key, value) DictAddImpl(dict, key, FindType(value))

enum DictTypes {
    DICT_STRING,
    DICT_INTEGER,
    DICT_FLOAT,
    DICT_DOUBLE,
    DICT_UNKNOWN
};

typedef union {
    char** strings;
    int* integers;
    double* doubles;
    float* floats;
} DictValueTypes;

typedef union {
    char* string;
    int integer;
    double doubleVal;
    float floatVal;
} DictValue;

typedef struct {
    char** keys;
    DictValueTypes values;
    int size;
    int occupied;
    enum DictTypes type;
} Dict;

typedef struct {
    DictValue value;
    enum DictTypes type;
} DictValueResponse;

void DictIntialize(Dict* dict, int size, int maxStringSize, enum DictTypes type) {
    dict->keys = malloc(sizeof(char*)*size);
    dict->size = size;
    dict->type = type;

    switch (dict->type) {
        case DICT_STRING:
            dict->values.strings = malloc(sizeof(char*)*size);
            break;
        case DICT_INTEGER:
            dict->values.integers = malloc(sizeof(int)*size);
            break;
        case DICT_DOUBLE:
            dict->values.doubles = malloc(sizeof(double)*size);
            break;
        case DICT_FLOAT:
            dict->values.floats = malloc(sizeof(float)*size);
            break;
    };

    for (int i = 0; i < size; i++) {
        dict->keys[i] = malloc(maxStringSize);

        if (dict->type == DICT_STRING) {
            dict->values.strings[i] = malloc(maxStringSize);

        }
    }
}

bool DictAddImpl(Dict* dict, char* key, DictValue value) {
    for (int i = 0; i < dict->occupied; i++) {
        if (!strcmp(dict->keys[i], key)) {
            return false;
        }
    }

    if (dict->occupied < dict->size) {
        strcpy(dict->keys[dict->occupied], key);

        switch (dict->type) {
            case DICT_STRING:
                strcpy(dict->values.strings[dict->occupied], value.string);
                break;
            case DICT_INTEGER:
                dict->values.integers[dict->occupied] = value.integer;
                break;
            case DICT_DOUBLE:
                dict->values.doubles[dict->occupied] = value.doubleVal;
                break;
            case DICT_FLOAT:
                dict->values.floats[dict->occupied] = value.floatVal;
                break;
        };

        dict->occupied++;
        return true;
    }

    return false;
}

DictValueResponse DictGetValueAtKey(Dict* dict, char* key) {
    for (int i = 0; i < dict->occupied; i++) {
        if (!strcmp(dict->keys[i], key)) {
            switch (dict->type) {
                case DICT_STRING:
                    return (DictValueResponse) { (DictValue) { .string = dict->values.strings[i] }, dict->type };
                case DICT_INTEGER:
                    return (DictValueResponse) { (DictValue) { .integer = dict->values.integers[i] }, dict->type };
                case DICT_DOUBLE:
                    return (DictValueResponse) { (DictValue) { .doubleVal = dict->values.doubles[i] }, dict->type };
                case DICT_FLOAT:
                    return (DictValueResponse) { (DictValue) { .floatVal = dict->values.floats[i] }, dict->type };
            };

        }
    }

    return (DictValueResponse) { 0, DICT_UNKNOWN };
}

bool DictRemoveKeyValuePair(Dict* dict, char* key) {
    for (int i = 0; i < dict->occupied; i++) {
        if (!strcmp(dict->keys[i], key)) {
            memmove(dict->keys+i, dict->keys+i+1, dict->occupied-i);

            switch (dict->type) {
                case DICT_STRING:
                    memmove(dict->values.strings+i, dict->values.strings+i+1, dict->occupied-i);
                    break;
                case DICT_INTEGER:
                    memmove(dict->values.integers+i, dict->values.integers+i+1, dict->occupied-i);
                    break;
                case DICT_DOUBLE:
                    memmove(dict->values.doubles+i, dict->values.doubles+i+1, dict->occupied-i);
                    break;
                case DICT_FLOAT:
                    memmove(dict->values.floats+i, dict->values.floats+i+1, dict->occupied-i);
                    break;
            };

            return true;
        }
    }

    return false;
}

int main(void) {
    Dict dict = {};
    DictIntialize(&dict, 5, 50, DICT_FLOAT);

    printf("got here.\n");
    printf("Success: `%d`\n", DictAdd(&dict, "1", 10.10f));
    printf("Success: `%d`\n", DictAdd(&dict, "2", 12.17f));
    printf("Success: `%d`\n", DictAdd(&dict, "3", 14.19f));
    printf("Success: `%d`\n", DictAdd(&dict, "4", 16.21f));
    printf("value: `%.3f`\n", DictGetValueAtKey(&dict, "2").value.floatVal);
    printf("successfully removed 2: `%d`\n", DictRemoveKeyValuePair(&dict, "2"));
    printf("value: `%.3f`\n", DictGetValueAtKey(&dict, "2").value.floatVal);
    printf("successfully removed 2: `%d`\n", DictRemoveKeyValuePair(&dict, "2"));

    return 0;
}
