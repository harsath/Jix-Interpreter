#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASH_TABLE_CAPACITY 100

typedef struct hash_node {
  char *key;
  void *value;
  struct hash_node *next;
} hash_node;

typedef struct {
  hash_node *buckets[HASH_TABLE_CAPACITY];
} hash_table;

unsigned int hash(const char *key);
hash_table *hash_table_init();
void *hash_table_lookup(hash_table *table, const char *key);
void hash_table_insert(hash_table *table, char *key, void *value);
void hash_table_update(hash_table *table, char *key, void *value);
void hash_table_delete(hash_table *table, const char *key);
void hash_table_free(hash_table *table);

#endif
