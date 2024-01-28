#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASH_TABLE_CAPACITY 100

struct hash_node {
  char *key;
  void *value;
  struct hash_node *next;
};

struct hash_table {
  struct hash_node *buckets[HASH_TABLE_CAPACITY];
};

unsigned int hash(const char *key);
struct hash_table *hash_table_init();
void *hash_table_lookup(struct hash_table *table, const char *key);
void hash_table_insert(struct hash_table *table, char *key, void *value);
void hash_table_update(struct hash_table *table, char *key, void *value);
void hash_table_delete(struct hash_table *table, const char *key);
void hash_table_free(struct hash_table *table);

#endif
