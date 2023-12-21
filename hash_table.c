#include "hash_table.h"

unsigned int hash(const char *key) {
  unsigned int hash = 0;
  for (int i = 0; key[i] != '\0'; i++) {
    hash = hash * 31 + key[i];
  }
  return hash % HASH_TABLE_CAPACITY;
}

hash_table *hash_table_init() {
  hash_table *table = malloc(sizeof(hash_table));
  if (!table) {
    return NULL;
  }
  for (size_t i = 0; i < HASH_TABLE_CAPACITY; i++) {
    table->buckets[i] = NULL;
  }
  return table;
}

void *hash_table_lookup(hash_table *table, const char *key) {
  unsigned int index = hash(key);
  hash_node *node = table->buckets[index];
  while (node) {
    if (strcmp(node->key, key) == 0) {
      return node->value;
    }
    node = node->next;
  }
  return NULL;
}

void hash_table_insert(hash_table *table, const char *key, void *value) {
  unsigned int index = hash(key);
  hash_node *new_node = malloc(sizeof(hash_node));
  if (!new_node) {
    perror("Hash table node allocation memory error\n");
    exit(1);
  }
  new_node->key = strdup(key);
  new_node->value = value;
  new_node->next = table->buckets[index];
  table->buckets[index] = new_node;
}

void hash_table_delete(hash_table *table, const char *key) {
  unsigned int index = hash(key);
  hash_node *node = table->buckets[index];
  hash_node *prev = NULL;
  while (node) {
    if (strcmp(node->key, key) == 0) {
      if (prev) {
        prev->next = node->next;
      } else {
        table->buckets[index] = node->next;
      }
      free(node->key);
      free(node);
      return;
    }
    prev = node;
    node = node->next;
  }
}

void hash_table_free(hash_table *table) {
  for (size_t i = 0; i < HASH_TABLE_CAPACITY; i++) {
    hash_node *node = table->buckets[i];
    while (node) {
      hash_node *next = node->next;
      free(node->key);
      free(node);
      node = next;
    }
  }
  free(table);
}
