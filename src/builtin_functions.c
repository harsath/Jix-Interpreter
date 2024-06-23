#include "builtin_functions.h"
#include "interpreter.h"

struct hash_table *init_and_register_builtin_fns() {
  struct hash_table *fn_table = hash_table_init();
  for (size_t i = 0; i < NUM_BUILTIN_FNS; i++) {
    struct builtin_fn *builtin_fns = malloc(sizeof(struct builtin_fn));
    builtin_fns[i].fn_name = builtin_fn_names[i];
    builtin_fns[i].fn_ptr = builtin_print;
    builtin_fns[i].num_parameters = 1;
    hash_table_insert(fn_table, builtin_fn_names[i], builtin_fns);
  }
  return fn_table;
}

struct builtin_fn *lookup_builtin_fns(struct hash_table *table,
                                      const char *key) {
  return hash_table_lookup(table, key);
}

void *builtin_print(struct object *value) {
  switch (value->data_type) {
  case INT_VALUE:
    printf("%li", value->int_value);
    break;
  case BOOLEAN_VALUE:
    printf("%b", value->bool_value);
    break;
  case STRING_VALUE:
    printf("%s", value->string_value);
    break;
  case ARRAY_VALUE:
    printf("[ ");
    for (size_t i = 0; i < value->array_value->size; i++) {
      builtin_print(vector_at(value->array_value, i));
      if (i != value->array_value->size - 1) {
        printf(",");
      }
      printf(" ");
    }
    printf("]");
    break;
  default:
    printf("Invalid datatype in print() function.");
    exit(1);
  }
  return NULL;
}
