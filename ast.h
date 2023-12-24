#ifndef AST_H
#define AST_H

#include "tokens.h"
#include "vector.h"
#include <stdbool.h>

enum ast_node_type {
  BINARY_NODE,
  PRIMARY_NODE,
  VARIABLE_DECL_STMT,
  VARIABLE_ASSIGN_STMT,
  IF_STMT,
  WHILE_STMT,
  BLOCK_STMT
};

enum ast_primary_node_type {
  NUMBER_PRIMARY_NODE,
  STRING_PRIMARY_NODE,
  IDENTIFIER_PRIMARY_NODE,
  BOOLEAN_PRIMARY_NODE,
  NIL_PRIMARY_NODE
};

struct ast_node {
  enum ast_node_type node_type;
  enum ast_primary_node_type primary_node_type;

  /* Variable decl statement */
  enum token_type var_decl_stmt_dtype;
  struct ast_node *var_decl_stmt_id;
  struct ast_node *var_decl_stmt_expr;

  /* Variable assign statement */
  struct ast_node *assign_stmt_id;
  struct ast_node *assign_stmt_expr;

  /* If statement */
  struct ast_node *if_stmt_expr;
  struct ast_node *if_stmt_block;
  struct ast_node *if_else_stmt_block;

  /* While statement */
  struct ast_node *while_stmt_expr;
  struct ast_node *while_stmt_block;

  /* Block statement */
  struct vector *block_stmt_stmts;

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
  enum token_type op;
};

#endif
