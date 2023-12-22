#ifndef AST_H
#define AST_H

#include <stdbool.h>
#include "tokens.h"
#include "vector.h"

typedef enum { BINARY_NODE, PRIMARY_NODE, VARIABLE_DECL_STMT, VARIABLE_ASSIGN_STMT, BLOCK_STMT } ast_node_type;

typedef enum {
  NUMBER_PRIMARY_NODE,
  STRING_PRIMARY_NODE,
  IDENTIFIER_PRIMARY_NODE,
  BOOLEAN_PRIMARY_NODE,
  NIL_PRIMARY_NODE
} ast_primary_node_type;

typedef struct ast_node {
  ast_node_type node_type;
  ast_primary_node_type primary_node_type;

  /* Variable decl statement */
  token_type var_decl_stmt_dtype;
  struct ast_node *var_decl_stmt_id;
  struct ast_node *var_decl_stmt_expr;

  /* Variable assign statement */
  struct ast_node *assign_stmt_id;
  struct ast_node *assign_stmt_expr;

  /* Block statement */
  vector *block_stmt_stmts;

  /* Number node */
  long number_value;

  /* String node */
  char *string_value;

  /* Identifier node */
  char *identifier_value;

  /* Boolean node */
  bool boolean_value;

  /* Binary node */
  struct ast_node *left;
  struct ast_node *right;
  token_type op;
} ast_node;

#endif
