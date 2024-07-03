#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "ast.h"
#include "errors.h"
#include "hash_table.h"
#include "tokens.h"
#include "vector.h"

#define RETURN_RESULT_IF_ERROR(return_value)                                   \
  do {                                                                         \
    if (return_value->type == RESULT_ERROR) {                                  \
      return return_value;                                                     \
    }                                                                          \
  } while (0)

struct environment {
  struct hash_table *symbols; /* Key: char*, Value: object* */
  struct environment *parent_environment;
};

struct interpreter_state {
  bool is_break;
  struct environment *env;
  struct hash_table *builtin_fns;
};

/* See documentation "docs/variable references.md" */
enum object_type {
  /* Immutable */
  INT_VALUE,
  BOOLEAN_VALUE,
  STRING_VALUE,
  FUNCTION_VALUE,
  NIL_VALUE,
  /* Mutable */
  ARRAY_VALUE,
};

struct object {
  enum object_type data_type;
  union {
    /* Immutable */
    long int_value;
    bool bool_value;
    char *string_value;
    struct {
      bool is_builtin;
      union {
        struct builtin_fn *builtin_function;
        struct function *function_value;
      };
    } function_value;
    /* Mutable */
    struct vector *array_value;
  };
};

struct return_value {
  bool is_set;
  struct result *value;
};

struct function {
  struct vector *parameters;
  struct ast_node *body;
};

struct object *interpret(struct vector *program);
struct result *interpret_statement(struct ast_node *statement,
                                   struct interpreter_state *state,
                                   struct return_value *return_code);
struct result *interpret_fn_def_statement(struct ast_node *stmt_node,
                                          struct interpreter_state *state);
struct result *interpret_expr_statement(struct ast_node *stmt_node,
                                        struct interpreter_state *state,
                                        struct return_value *return_code);
struct result *interpret_return_statement(struct ast_node *stmt_node,
                                          struct interpreter_state *state,
                                          struct return_value *return_code);
struct result *
interpret_variable_decl_statement(struct ast_node *stmt_node,
                                  struct interpreter_state *state,
                                  struct return_value *return_code);
struct result *
interpret_variable_assignment_statement(struct ast_node *stmt_node,
                                        struct interpreter_state *state,
                                        struct return_value *return_code);
struct result *interpret_if_statement(struct ast_node *stmt_node,
                                      struct interpreter_state *state,
                                      struct return_value *return_code);
struct result *interpret_while_statement(struct ast_node *stmt_node,
                                         struct interpreter_state *state,
                                         struct return_value *return_code);
struct result *interpret_for_statement(struct ast_node *stmt_node,
                                       struct interpreter_state *state,
                                       struct return_value *return_code);
struct result *interpret_break_statement(struct ast_node *stmt_node,
                                         struct interpreter_state *state,
                                         struct return_value *return_code);
struct result *interpret_block_statement(struct ast_node *stmt_node,
                                         struct interpreter_state *state,
                                         struct return_value *return_code);

struct result *eval_expression(struct ast_node *ast,
                               struct interpreter_state *state,
                               struct return_value *return_code);
struct result *eval_binary_expression(struct ast_node *ast,
                                      struct interpreter_state *state,
                                      struct return_value *return_code);
struct result *eval_unary_expression(struct ast_node *ast,
                                     struct interpreter_state *state,
                                     struct return_value *return_code);
struct result *eval_logical_expression(enum token_type op, struct object *lhs,
                                       struct object *rhs);
struct result *eval_equality_expression(enum token_type op, struct object *lhs,
                                        struct object *rhs);
struct result *eval_comparitive_expression(enum token_type op,
                                           struct object *lhs,
                                           struct object *rhs);
struct result *eval_additive_multiplicative_expression(enum token_type op,
                                                       struct object *lhs,
                                                       struct object *rhs);
struct result *eval_primary_expression(struct ast_node *ast,
                                       struct interpreter_state *state,
                                       struct return_value *return_code);
struct result *
eval_fn_call_primary_expression(struct ast_node *ast,
                                struct interpreter_state *state,
                                struct return_value *return_code);
struct result *eval_builtin_fn_call_primary_expression(
    struct ast_node *ast, struct object *fn_call_primary,
    struct interpreter_state *state, struct return_value *return_code);
struct result *
eval_method_call_primary_expression(struct ast_node *ast,
                                    struct interpreter_state *state,
                                    struct return_value *return_code);
struct result *
eval_array_creation_primary_expression(struct ast_node *ast,
                                       struct interpreter_state *state,
                                       struct return_value *return_code);
struct result *
eval_array_access_primary_expression(struct ast_node *ast,
                                     struct interpreter_state *state,
                                     struct return_value *return_code);

struct environment *environment_init();
struct environment *environment_init_enclosed(struct environment *enclosed_env);
struct object *environment_lookup_symbol(struct environment *env, char *key);
struct object *environment_lookup_symbol_current_env(struct environment *env,
                                                     char *key);
void environment_insert_symbol(struct environment *env, char *key,
                               struct object *value);
void environment_reassign_symbol(struct environment *env, char *key,
                                 struct object *value);
void environment_free(struct environment *env);

#endif
