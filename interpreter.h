#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "ast.h"
#include "hash_table.h"
#include "tokens.h"
#include "vector.h"

struct environment {
  struct hash_table *current_env_variables;
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

struct object *interpret(struct vector *program);
void interpret_statement(struct ast_node *ast, struct interpreter_state *state,
                         struct object *return_code);
void interpret_variable_decl_statement(struct ast_node *stmt_node,
                                       struct interpreter_state *state);
void interpret_variable_assignment_statement(struct ast_node *stmt_node,
                                             struct interpreter_state *state);
void interpret_if_statement(struct ast_node *stmt_node,
                            struct interpreter_state *state,
                            struct object *return_code);
void interpret_while_statement(struct ast_node *stmt_node,
                            struct interpreter_state *state,
                            struct object *return_code);
void interpret_block_statement(struct ast_node *stmt_node,
                               struct interpreter_state *state,
                               struct object *return_code);
struct object *eval_expression(struct ast_node *ast,
                               struct interpreter_state *state);
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
struct object *environment_lookup(struct environment *env, char *key);
struct object *environment_lookup_current_env(struct environment *env,
                                              char *key);
void environment_insert(struct environment *env, char *key,
                        struct object *value);
void environment_update(struct environment *env, char *key,
                        struct object *value);
void environment_free(struct environment *env);

#endif
