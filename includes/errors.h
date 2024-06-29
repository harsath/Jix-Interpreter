#ifndef ERRORS_H
#define ERRORS_H

#include "ast.h"

enum error_type { ERROR_SYNTAX, ERROR_RUNTIME };

struct error {
  enum error_type type;
  const char *message;
  size_t line;
  size_t column;
};

enum result_type { RESULT_OK, RESULT_ERROR };

struct result {
  enum result_type type;
  union {
    struct ast_node *node;
    struct object *object;
    struct error *error;
  };
};

struct error *error_init(enum error_type type, const char *message, size_t line,
                         size_t column);

struct result *result_ok_node(struct ast_node *node);
struct result *result_ok_object(struct object *object);
struct result *result_error(struct error *error);

#endif
