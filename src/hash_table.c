#include "hash_table.h"

unsigned int hash(const char *key) {
  unsigned int hash = 0;
  for (int i = 0; key[i] != '\0'; i++) {
    hash = hash * 31 + key[i];
  }
  return hash % HASH_TABLE_CAPACITY;
}

struct hash_table *hash_table_init() {
  struct hash_table *table = malloc(sizeof(struct hash_table));
  if (!table) {
    return NULL;
  }
  for (size_t i = 0; i < HASH_TABLE_CAPACITY; i++) {
    table->buckets[i] = NULL;
  }
  return table;
}

void *hash_table_lookup(struct hash_table *table, const char *key) {
  unsigned int index = hash(key);
  struct hash_node *node = table->buckets[index];
  while (node) {
    if (strcmp(node->key, key) == 0) {
      return node->value;
    }
    node = node->next;
  }
  return NULL;
}

void hash_table_insert(struct hash_table *table, const char *key, void *value) {
  unsigned int index = hash(key);
  struct hash_node *new_node = malloc(sizeof(struct hash_node));
  if (!new_node) {
    perror("Hash table node allocation memory error\n");
    exit(1);
  }
  new_node->key = key;
  new_node->value = value;
  new_node->next = table->buckets[index];
  table->buckets[index] = new_node;
}

void hash_table_update(struct hash_table *table, const char *key, void *value) {
  unsigned int index = hash(key);
  struct hash_node *node = table->buckets[index];
  while (node) {
    if (strcmp(node->key, key) == 0) {
      node->value = value;
      return;
    }
    node = node->next;
  }
}

void hash_table_delete(struct hash_table *table, const char *key) {
  unsigned int index = hash(key);
  struct hash_node *node = table->buckets[index];
  struct hash_node *prev = NULL;
  while (node) {
    if (strcmp(node->key, key) == 0) {
      if (prev) {
        prev->next = node->next;
      } else {
        table->buckets[index] = node->next;
      }
      free(node);
      return;
    }
    prev = node;
    node = node->next;
  }
}

void hash_table_free(struct hash_table *table) {
  for (size_t i = 0; i < HASH_TABLE_CAPACITY; i++) {
    struct hash_node *node = table->buckets[i];
    while (node) {
      struct hash_node *next = node->next;
      free(node);
      node = next;
    }
  }
  free(table);
}
