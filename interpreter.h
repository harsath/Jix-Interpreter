#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "ast.h"
#include "tokens.h"
#include "hash_table.h"
#include "vector.h"

typedef struct { 
  hash_table *variables;
} interpreter_state;

typedef struct {
  token_type data_type;
  
  long int_value;
  bool bool_value;
  char *string_value;
} object;

object *interpret(vector *program);
void interpret_statement(ast_node *ast, interpreter_state *state, object *return_code);
object *eval_expression(ast_node *ast, interpreter_state *state);


#endif
