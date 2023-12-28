#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "ast.h"
#include "hash_table.h"
#include "tokens.h"
#include "vector.h"

struct environment {
  struct hash_table *current_env_variables; /* Key: char*, Value: object* */
  struct hash_table *current_env_functions; /* Key: char*, Value: function* */
  struct environment *enclosing_environment;
};

struct interpreter_state {
  struct environment *env;
};

struct object {
  enum token_type data_type;
  long int_value;
  bool bool_value;
  char *string_value;
};

struct return_value {
  bool is_set;
  struct object *value;
};

struct function {
  enum token_type return_type;
  struct vector *parameters; /* Vector of `struct ast_fn_def_parameter' type. */
  struct ast_node *body;     /* Block statement */
};

struct object *interpret(struct vector *program);
void interpret_statement(struct ast_node *ast, struct interpreter_state *state,
                         struct return_value *return_code);
void interpret_fn_def_statement(struct ast_node *stmt_node,
                                struct interpreter_state *state);
void interpret_expr_statement(struct ast_node *stmt_node,
                              struct interpreter_state *state,
                              struct return_value *return_code);
void interpret_return_statement(struct ast_node *stmt_node,
                                struct interpreter_state *state,
                                struct return_value *return_code);
void interpret_variable_decl_statement(struct ast_node *stmt_node,
                                       struct interpreter_state *state,
                                       struct return_value *return_code);
void interpret_variable_assignment_statement(struct ast_node *stmt_node,
                                             struct interpreter_state *state,
                                             struct return_value *return_code);
void interpret_if_statement(struct ast_node *stmt_node,
                            struct interpreter_state *state,
                            struct return_value *return_code);
void interpret_while_statement(struct ast_node *stmt_node,
                               struct interpreter_state *state,
                               struct return_value *return_code);
void interpret_block_statement(struct ast_node *stmt_node,
                               struct interpreter_state *state,
                               struct return_value *return_code);
struct object *eval_expression(struct ast_node *ast,
                               struct interpreter_state *state,
                               struct return_value *return_value);
struct object *eval_logical_expression(enum token_type op, struct object *lhs,
                                       struct object *rhs);
struct object *eval_equality_expression(enum token_type op, struct object *lhs,
                                        struct object *rhs);
struct object *eval_comparitive_expression(enum token_type op,
                                           struct object *lhs,
                                           struct object *rhs);
struct object *eval_additive_multiplicative_expression(enum token_type op,
                                                       struct object *lhs,
                                                       struct object *rhs);
struct object *eval_primary_expression(struct ast_node *ast,
                                       struct interpreter_state *state);

struct environment *environment_init();
struct environment *environment_init_enclosed(struct environment *enclosed_env);
struct object *environment_lookup_variable(struct environment *env, char *key);
struct object *environment_lookup_variable_current_env(struct environment *env,
                                                       char *key);
void environment_insert_variable(struct environment *env, char *key,
                                 struct object *value);
void environment_update_variable(struct environment *env, char *key,
                                 struct object *value);
struct function *environment_lookup_function(struct environment *env,
                                             char *key);
struct function *
environment_lookup_function_current_env(struct environment *env, char *key);
void environment_insert_function(struct environment *env, char *key,
                                 struct function *value);
void environment_free(struct environment *env);

// void set_return_value_from_object(struct return_value *ret, struct object
// *obj);

#endif
