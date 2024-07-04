#ifndef ERRORS_H
#define ERRORS_H

#include "ast.h"

struct parser_error {
  const char *message;
  size_t line;
};

struct runtime_error {
  const char *message;
  size_t start_line;
  size_t end_line;
};

enum result_type { RESULT_OK, RESULT_ERROR };

struct result {
  enum result_type type;
  union {
    struct ast_node *node;
    struct object *object;
    union {
      struct parser_error *parser;
      struct runtime_error *runtime;
    } error;
  };
};

struct parser_error *parser_error_init(const char *message, size_t line);
struct runtime_error *runtime_error_init(const char *message, size_t start_line,
                                         size_t end_line);

struct result *result_ok_node(struct ast_node *node);
struct result *result_ok_object(struct object *object);

struct result *result_error_parser(struct parser_error *error);
struct result *result_error_runtime(struct runtime_error *error);

#endif
