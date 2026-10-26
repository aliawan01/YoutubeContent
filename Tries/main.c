#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef int64_t i64;
typedef int32_t i32;
typedef int16_t i16;
typedef int8_t i8;

typedef double f64;
typedef float f32;

typedef u32 b32;

#define true 1
#define false 0

#define ArrayCount(x) (sizeof(x)/sizeof((x)[0]))
#define strlit(x) ((String) { .string = x, .count = sizeof(x)-1 })

typedef struct String String;
struct String {
  String* next;

  char* string;
  u32 count;
};

typedef struct TrieNode TrieNode;
typedef struct TrieNodeChild TrieNodeChild;

struct TrieNodeChild {
  TrieNodeChild* next;

  char character;
  TrieNode* node;
};

struct TrieNode {
  String word;
  b32 is_word;

  // branches/children 
  TrieNodeChild* children;
};

typedef struct {
  TrieNode* root;
} Trie;

Trie trie_create(String* strings, u32 strings_count) {
  Trie trie = {
    .root = calloc(1, sizeof(TrieNode))
  };

  for (u32 i = 0; i < strings_count; i++) {
    String current_string = strings[i];
    TrieNode* current_node = trie.root;

    for (u32 x = 0; x < current_string.count; x++) {
      /*
      root
      children = NULL -> malloc/calloc childnode character alloc  allocate node it points to (fill word)
                        current_node = current_node->children[x].node;
      children != NULL -> a, b, c, d, next -> NULL
           c -> current_node = current_node->children[x].node;
           e -> malloc/calloc childnode character alloc  allocate node it points to (fill word)
               current_node = current_node->children[x].node;
      */

      TrieNodeChild** child = &current_node->children;

      while (*child != NULL && (*child)->character != current_string.string[x])
        child = &((*child)->next);

      if (*child == NULL) {
        *child = calloc(1, sizeof(TrieNodeChild));
        (*child)->character = current_string.string[x];
        (*child)->node = calloc(1, sizeof(TrieNode));
        (*child)->node->word = (String) {
          .string = calloc(x+2, sizeof(char)),
          .count = x+1
        };

        memcpy((*child)->node->word.string, current_string.string, sizeof(char)*(x+1));
      }

      current_node = (*child)->node;

      if (x == current_string.count-1)
        current_node->is_word = true;
    }
  }

  return trie;
}

b32 trie_word_exists(Trie trie, String word) {
  TrieNode* current_node = trie.root;

  for (u32 i = 0; i < word.count; i++) {
    TrieNodeChild* child = current_node->children;

    while (child != NULL && child->character != word.string[i])
      child = child->next;

    if (child == NULL) 
      return false;

    current_node = child->node;
  }

  return current_node->is_word;
}

void __trie_scan_subtree_for_words(TrieNode* node, String* words) {
  if (node->is_word) {
    words->count = node->word.count;
    words->string = node->word.string;
    words->next = calloc(1, sizeof(String));
    words = words->next;
  }

  TrieNodeChild* child = node->children;
  for (; child != NULL; child = child->next) {
    while (words->next != NULL)
      words = words->next;

    __trie_scan_subtree_for_words(child->node, words);
  }
}

String trie_starts_with(Trie trie, String word) {
  TrieNode* current_node = trie.root;
  String prefix_strings = {0};

  for (u32 i = 0; i < word.count; i++) {
    TrieNodeChild* child = current_node->children;

    while (child != NULL && child->character != word.string[i])
      child = child->next;

    if (child == NULL) 
      return prefix_strings;

    current_node = child->node;
  }

  // word -> back
  __trie_scan_subtree_for_words(current_node, &prefix_strings);

  return prefix_strings;
}

int main(void) {
  String word_list[] = {
    strlit("app"),
    strlit("apple"),
    strlit("back"),
    strlit("backs"),
    strlit("backwords"),
    strlit("doctor"),
    strlit("cats")
  };

  Trie trie = trie_create(word_list, ArrayCount(word_list));

  printf("trie_word_exists: %d\n", trie_word_exists(trie, strlit("app")));

  String strings = trie_starts_with(trie, strlit("back"));
  for (String* current_string = &strings; current_string->next != NULL; current_string = current_string->next) {
    printf("%s\n", current_string->string);
  }

  return 0;
}
