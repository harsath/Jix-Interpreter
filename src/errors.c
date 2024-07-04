#include "errors.h"

struct parser_error *parser_error_init(const char *message, size_t line) {
  struct parser_error *ret = malloc(sizeof(struct parser_error));
  ret->message = message;
  ret->line = line;
  return ret;
}

struct runtime_error *runtime_error_init(const char *message, size_t start_line,
                                         size_t end_line) {
  struct runtime_error *ret = malloc(sizeof(struct runtime_error));
  ret->message = message;
  ret->start_line = start_line;
  ret->end_line = end_line;
  return ret;
}

struct result *result_ok_node(struct ast_node *node) {
  struct result *ret = malloc(sizeof(struct result));
  ret->type = RESULT_OK;
  ret->node = node;
  return ret;
}

struct result *result_ok_object(struct object *object) {
  struct result *ret = malloc(sizeof(struct result));
  ret->type = RESULT_OK;
  ret->object = object;
  return ret;
}

struct result *result_error_parser(struct parser_error *error) {
  struct result *ret = malloc(sizeof(struct result));
  ret->type = RESULT_ERROR;
  ret->error.parser = error;
  return ret;
}

struct result *result_error_runtime(struct runtime_error *error) {
  struct result *ret = malloc(sizeof(struct result));
  ret->type = RESULT_ERROR;
  ret->error.runtime = error;
  return ret;
}
