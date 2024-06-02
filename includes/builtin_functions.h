#ifndef BUILTIN_FUNCTIONS_H
#define BUILTIN_FUNCTIONS_H

#include <stdio.h>
#include "hash_table.h"
#include "interpreter.h"
#define NUM_BUILTIN_FNS 1

void builtin_print(struct object*);

struct builtin_fn {
  void *fn_ptr;
  size_t num_parameters;
};

struct hash_table *init_and_register_builtin_fns();
struct builtin_fn *lookup_builtin_fns(struct hash_table *table, const char *key);

#endif
