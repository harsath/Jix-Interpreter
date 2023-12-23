#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "ast.h"
#include "hash_table.h"
#include "tokens.h"
#include "vector.h"

typedef struct environment {
  hash_table *current_env_variables;
  struct environment *enclosing_environment; 
} environment;


typedef struct {
  environment *env;
} interpreter_state;

typedef struct {
  token_type data_type;

  long int_value;
  bool bool_value;
  char *string_value;
} object;


object *interpret(vector *program);
void interpret_statement(ast_node *ast, interpreter_state *state,
                         object *return_code);
void interpret_variable_decl_statement(ast_node *stmt_node, interpreter_state *state);
void interpret_variable_assignment_statement(ast_node *stmt_node, interpreter_state *state);
void interpret_block_statement(ast_node *stmt_node, interpreter_state *state, object *return_code);
object *eval_expression(ast_node *ast, interpreter_state *state);

environment *environment_init();
environment *environment_init_enclosed(environment *enclosed_env);
object *environment_lookup(environment *env, char *key);
object *environment_lookup_current_env(environment *env, char *key);
void environment_insert(environment *env, char *key, object *value);
void environment_update(environment *env, char *key, object *value);
void environment_free(environment *env);


#endif
