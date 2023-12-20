#ifndef AST_H
#define AST_H

#include "tokens.h"

typedef enum { BINARY_NODE, PRIMARY_NODE } ast_node_type;

typedef enum {
  NUMBER_PRIMARY_NODE,
  STRING_PRIMARY_NODE,
  BOOLEAN_PRIMARY_NODE
} ast_primary_node_type;

typedef struct ast_node {
  ast_node_type node_type;
  ast_primary_node_type primary_node_type;

  /* Number node */
  long number_value;

  /* Binary node */
  struct ast_node *left;
  struct ast_node *right;
  token_type op;
} ast_node;

#endif
