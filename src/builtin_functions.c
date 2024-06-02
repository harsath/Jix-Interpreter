#include "builtin_functions.h"
#include "interpreter.h"

struct hash_table *init_and_register_builtin_fns() {
  struct hash_table *fn_table = hash_table_init();
  if (!fn_table) {
    return NULL;
  }

  struct builtin_fn *builtin_fns = malloc(sizeof(struct builtin_fn) * NUM_BUILTIN_FNS);
  builtin_fns[0].fn_ptr = builtin_print;
  builtin_fns[0].num_parameters = 1;
  hash_table_insert(fn_table, strdup("print"), builtin_fns);
  return fn_table;
}

struct builtin_fn *lookup_builtin_fns(struct hash_table *table, const char *key) {
  return hash_table_lookup(table, key);
}

void builtin_print(struct object *value) {
  switch (value->data_type) {
  case INT_VALUE:
    printf("%li\n", value->int_value);
    break;
  case BOOLEAN_VALUE:
    printf("%b\n", value->bool_value);
    break;
  case STRING_VALUE:
    printf("%s\n", value->string_value);
    break;
  default:
    printf("Invalid datatype in print() function.");
    exit(1);
  }
}
