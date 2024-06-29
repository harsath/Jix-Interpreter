#include "errors.h"

/**
 * For ERROR_RUNTIME, `expected` will contain the error message and `got` will
 * be NULL.
 *
 * For ERROR_SYNTAX, `expected` will be the string representation of the token
 * that is expected and `got` will be the token that we received intead.
 */
struct error *error_init(enum error_type type, const char *message, size_t line,
                         size_t column) {
  struct error *ret = malloc(sizeof(struct error));
  ret->type = type;
  ret->message = message;
  ret->line = line;
  ret->column = column;
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

struct result *result_error(struct error *error) {
  struct result *ret = malloc(sizeof(struct result));
  ret->type = RESULT_ERROR;
  ret->error = error;
  return ret;
}
