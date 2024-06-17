#include "string_builder.h"

struct string_builder *string_builder_init() {
  struct string_builder *str_builder = malloc(sizeof(struct string_builder));
  str_builder->str = calloc(STRING_BUILDER_INIT_CAPACITY, sizeof(char));
  str_builder->capacity = STRING_BUILDER_INIT_CAPACITY;
  str_builder->length = 0;
  return str_builder;
}

void string_builder_append(struct string_builder *str_builder, const char *str) {
  size_t str_len = strlen(str);
  size_t new_builder_len = str_builder->length + str_len;
  if (new_builder_len >= str_builder->capacity) {
    str_builder->capacity = (new_builder_len + 1) * 2;
    str_builder->str = realloc(str_builder->str, str_builder->capacity);
  }
  strlcat(str_builder->str, str, str_builder->capacity);
  str_builder->length = new_builder_len;
}

void string_builder_free(struct string_builder *str_builder) {
  free(str_builder->str);
  free(str_builder);
}
