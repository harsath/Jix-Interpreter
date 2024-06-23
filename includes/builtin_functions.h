#ifndef BUILTIN_FUNCTIONS_H
#define BUILTIN_FUNCTIONS_H

#include "hash_table.h"
#include "interpreter.h"
#include <stdio.h>
#define NUM_BUILTIN_FNS 1

void *builtin_print(struct object *);

static const char *builtin_fn_names[NUM_BUILTIN_FNS] = {"print"};
static void *builtin_fn_ptrs[NUM_BUILTIN_FNS] = {builtin_print};
static size_t builtin_fn_arities[NUM_BUILTIN_FNS] = {1};

struct builtin_fn {
  const char *fn_name;
  void *fn_ptr;
  size_t num_parameters;
};

struct hash_table *init_and_register_builtin_fns();
struct builtin_fn *lookup_builtin_fns(struct hash_table *table,
                                      const char *key);

#endif
