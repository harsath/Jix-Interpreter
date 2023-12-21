#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "ast.h"
#include "tokens.h"
#include "hash_table.h"

typedef struct { 
  hash_table *variables;
} interpreter_state;

typedef struct {
  token_type data_type;
  void *value;
} variable_value_node;

long interpret(ast_node *ast);
long interpret_ast(ast_node *ast, interpreter_state *state);


#endif
