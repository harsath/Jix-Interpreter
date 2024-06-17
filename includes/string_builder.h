#ifndef STRING_BUILDER_H
#define STRING_BUILDER_H

#include <string.h>
#include <stdlib.h>

#define STRING_BUILDER_INIT_CAPACITY 20

struct string_builder {
  char *str;
  size_t length;
  size_t capacity;
};

struct string_builder *string_builder_init();
void string_builder_append(struct string_builder *, const char *);
void string_builder_free(struct string_builder *);

#endif
