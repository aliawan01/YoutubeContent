#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "arena.h"

typedef struct {
    char* string;
    uint64_t len;
} String;

typedef struct StringNode {
    String string;
    struct StringNode* child;
} StringNode;

typedef struct {
    StringNode* node;
} StringArray;

#define StrLit(s) (String) {s, sizeof(s)-1}

String StringAppend(Arena* arena, String originalString, String appendString) {
    uint64_t stringLen = originalString.len + appendString.len;
    char* buffer = PushString(arena, stringLen);
    memcpy(buffer, originalString.string, originalString.len);
    memcpy(buffer+originalString.len, appendString.string, appendString.len);

    return (String) {buffer, stringLen};
}

char* StringToCString(Arena* arena, String string) {
    char* cstring = PushString(arena, string.len);
    memcpy(cstring, string.string, string.len);
    cstring[string.len] = 0;
    return cstring;
}

bool StringCmp(String firstString, String secondString) {
    if (firstString.len != secondString.len) {
        return false;
    }

    for (int i = 0; i < firstString.len; i++) {
        if (firstString.string[i] != secondString.string[i]) {
            return false;
        }
    }

    return true;
}

String StringRange(Arena* arena, String string, uint64_t startPos, uint64_t endPos) {
    if (startPos > endPos || endPos > string.len) {
        return (String) {NULL, -1};
    }

    return (String) {string.string+startPos, endPos-startPos};
}


String StringTrimWhitespace(String string) {
    int startIndex = 0;
    for (int i = 0; i < string.len; i++) {
        if (string.string[i] != ' ') {
            startIndex =  i;
            break;
        }
    }

    int endIndex = 0;
    for (int i = string.len-1; i > startIndex; i--) {
        if (string.string[i] != ' ') {
            endIndex = i+1;;
            break;
        }
    }

    return (String) {string.string+startIndex, endIndex-startIndex};
}


StringArray StringSepOnWhitespace(Arena* arena, String string) {
    String trimmedString = StringAppend(arena, StringTrimWhitespace(string), StrLit(" "));

    StringArray array = {};
    array.node = PushStruct(arena, StringNode);

    StringNode* currentNode = array.node;
    for (int i = 0, prevStartPos = 0; i < trimmedString.len; i++) {
        if (trimmedString.string[i] == ' ') {
            uint64_t stringLen = i-prevStartPos;
            char* buffer = PushString(arena, stringLen);
            memcpy(buffer, trimmedString.string+prevStartPos, stringLen);
            currentNode->string = (String) {buffer, stringLen};
            currentNode->child = PushStruct(arena, StringNode);
            currentNode = currentNode->child;
            prevStartPos = i+1;
        }
    }

    return array;
}


int main(void) {
    ctx = (ThreadContext) {
        .id = 1,
        .globalArena = malloc(sizeof(Arena)),
        .scratchPool[0] = malloc(sizeof(Arena)),
        .scratchPool[1] = malloc(sizeof(Arena))
    };

    ArenaInit(ctx.globalArena, KB(64));
    ArenaInit(ctx.scratchPool[0], KB(64));
    ArenaInit(ctx.scratchPool[1], KB(64));

    String firstString = StrLit(" a string which contains a lot of whitespace around the end          ");
    String secondString = StrLit(" second");
    String thirdString = StringAppend(ctx.globalArena, firstString, secondString);
    String otherString = StrLit("first");


    printf("`%s`\n", StringToCString(ctx.globalArena, thirdString));
    printf("%d\n", StringCmp(firstString, otherString));
    /* printf("`%s`\n", StringToCString(ctx.globalArena, StringRange(ctx.globalArena, firstString, 2, firstString.len))); */

    printf("`%s`\n", StringToCString(ctx.globalArena, firstString));
    printf("`%s`\n", StringToCString(ctx.globalArena, StringTrimWhitespace(firstString)));


    StringArray array = StringSepOnWhitespace(ctx.globalArena, firstString);

    for (StringNode* node = array.node; node->child != NULL; node = node->child) {
        printf("String: `%s`, Length: %lld\n", StringToCString(ctx.globalArena, node->string), node->string.len);
    }

    return 0;
}
