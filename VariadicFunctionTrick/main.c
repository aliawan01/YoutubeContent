#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "map.h"

enum VariadicTypes {
    VARIADIC_END,
    VARIADIC_INT16,
    VARIADIC_UINT16,
    VARIADIC_INT32,
    VARIADIC_UINT32,
    VARIADIC_INT64,
    VARIADIC_UINT64,
    VARIADIC_CHAR,
    VARIADIC_LONG_DOUBLE,
    VARIADIC_DOUBLE,
    VARIADIC_STRING,
    VARIADIC_NULL,
    VARIADIC_UNKNOWN
};

#define GetTypeVariadicArgs(value) _Generic((value), \
        char*                 : VARIADIC_STRING,     \
        short                 : VARIADIC_INT16,      \
        unsigned short        : VARIADIC_UINT16,     \
        int                   : VARIADIC_INT32,      \
        unsigned int          : VARIADIC_UINT32,     \
        long int              : VARIADIC_INT32,      \
        unsigned long int     : VARIADIC_UINT32,     \
        long long int         : VARIADIC_INT64,      \
        unsigned long long int: VARIADIC_UINT64,     \
        char                  : VARIADIC_CHAR,       \
        float                 : VARIADIC_DOUBLE,     \
        double                : VARIADIC_DOUBLE,     \
        long double           : VARIADIC_LONG_DOUBLE,\
        void*                 : VARIADIC_NULL,       \
        default               : VARIADIC_UNKNOWN     \
        ), (value)

#define print(formatString, ...) ExamplePrint(formatString, MAP_LIST(GetTypeVariadicArgs, ##__VA_ARGS__), VARIADIC_END);

typedef struct {
    int array[20];
    int count;
} IntArray;

void ExamplePrint(char* formatString, ...) {
    va_list args;
    va_start(args, formatString);

    char stringSplitIntoParts[10][50] = {0};
    int stringSplitIntoPartsIndex = 0;

    IntArray emptyIndexes = {0};

    int previousIndex = 0;
    for (int i = 0; i < strlen(formatString); i++) {
        if (formatString[i] == '%') {
            formatString[i] = 0;
            strcpy(stringSplitIntoParts[stringSplitIntoPartsIndex], formatString+previousIndex);
            formatString[i] = '%';
            previousIndex = i+1;

            stringSplitIntoPartsIndex += 2;

            emptyIndexes.array[emptyIndexes.count] = stringSplitIntoPartsIndex-1;
            emptyIndexes.count++;
        }
    }

    strcpy(stringSplitIntoParts[stringSplitIntoPartsIndex], formatString+previousIndex);

    bool finished = false;
    char nextString[512] = {0};

    for (int i = 0; i < emptyIndexes.count && !finished; i++) {
        switch (va_arg(args, int)) {
            case VARIADIC_INT16:
                sprintf(nextString, "%hd", va_arg(args, int16_t));
                break;
            case VARIADIC_UINT16:
                sprintf(nextString, "%hu", va_arg(args, uint16_t));
                break;
            case VARIADIC_INT32:
                sprintf(nextString, "%d", va_arg(args, int32_t));
                break;
            case VARIADIC_UINT32:
                sprintf(nextString, "%u", va_arg(args, uint32_t));
                break;
            case VARIADIC_INT64:
                sprintf(nextString, "%lld", va_arg(args, int64_t));
                break;
            case VARIADIC_UINT64:
                sprintf(nextString, "%llu", va_arg(args, uint64_t));
                break;
            case VARIADIC_CHAR:
                sprintf(nextString, "%c", va_arg(args, char));
                break;
            case VARIADIC_LONG_DOUBLE:
                sprintf(nextString, "%Lf", va_arg(args, long double)); 
                break;
            case VARIADIC_DOUBLE:
                sprintf(nextString, "%.2f", va_arg(args, double));
                break;
            case VARIADIC_STRING:
                sprintf(nextString, "%s", va_arg(args, char*));
                break;
            case VARIADIC_NULL: 
                va_arg(args, void*);
                sprintf(nextString, "%s", "(null)");
                continue;
                break;
            case VARIADIC_UNKNOWN:
                printf("[ERROR] Got VARIADIC_UNKNOWN at index: `%d`\n", i);
                finished = true;
                break;
            case VARIADIC_END:
                finished = true;
                break;
        };


        strcpy(stringSplitIntoParts[emptyIndexes.array[i]], nextString);
    }


    va_end(args);

    char finalString[1024] = {0};
    for (int i = 0; i < stringSplitIntoPartsIndex; i++) {
        strcat(finalString, stringSplitIntoParts[i]);
    }

    printf("%s\n", finalString);
}


int main(void) {
    print("% % %\n", "Ali", "Awan", 19.54);
    return 0;
}
