#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "arena.h"
#include "arena.c"

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

typedef struct {
    char*    buffer;
    uint64_t index;
    uint64_t len;
} StringBuilder;

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

String StringRange(Arena* arena, String string, uint64_t startPos, uint64_t endPos) {
    if (endPos > string.len || startPos > endPos) {
        return (String) { NULL, -1 };
    }

    return (String) { string.string+startPos, endPos-startPos };
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

bool StringSubstringExists(String originalString, String substring) {
    if (originalString.len < substring.len) {
        return false;
    }

    for (int i = 0; i < originalString.len-substring.len; i++) {
        String originalStringOffset = (String) {originalString.string+i, substring.len};
        if (StringCmp(originalStringOffset, substring)) {
            return true;
        }
    }

    return false;
}

String StringTrimWhitespace(String string) {
    int startIndex = 0;
    for (int i = 0; i < string.len; i++) {
        if (string.string[i] != ' ') {
            startIndex = i;
            break;
        }
    }

    int endIndex = 0;
    for (int i = string.len-1; i > startIndex; i--) {
        if (string.string[i] != ' ') {
            endIndex = i;
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
            currentNode->string = (String) { buffer, stringLen };
            currentNode->child = PushStruct(arena, StringNode);
            currentNode = currentNode->child;
            prevStartPos = i+1;
        }
    }

    return array;
}

void StringBuilderInit(Arena* arena, StringBuilder* builder, uint64_t maxSize) {
    *builder = (StringBuilder) {
        .buffer = PushString(arena, maxSize),
        .index = 0,
        .len = maxSize
    };
}

bool StringBuilderAppend(StringBuilder* builder, String appendString) {
    if (builder->index+appendString.len > builder->len) {
        return false;
    }

    memcpy(builder->buffer+builder->index, appendString.string, appendString.len);
    builder->index += appendString.len;
    return true;
}

bool StringBuilderRemove(StringBuilder* builder, uint64_t removeLen) {
    if (removeLen > builder->index) {
        return false;
    }

    builder->index -= removeLen;
    return true;
}

String StringBuilderToString(StringBuilder builder) {
    return (String) { builder.buffer, builder.index };
}

String StringBuilderCopyString(Arena* arena, StringBuilder builder) {
    char* stringCopy = PushString(arena, builder.index);
    memcpy(stringCopy, builder.buffer, builder.index);
    return (String) { stringCopy, builder.index };
}

char* StringBuilderToCString(Arena* arena, StringBuilder builder) {
    char* stringCopy = PushString(arena, builder.index+1);
    memcpy(stringCopy, builder.buffer, builder.index);
    return stringCopy;
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


    StringBuilder stringBuilder = {};
    StringBuilderInit(ctx.globalArena, &stringBuilder, KB(20));

    StringBuilderAppend(&stringBuilder, StrLit("hello world"));
    StringBuilderAppend(&stringBuilder, StrLit(" another string"));
    StringBuilderRemove(&stringBuilder, 100);

    printf("`%s`\n", StringBuilderToCString(ctx.globalArena, stringBuilder));

    return 0;
}
