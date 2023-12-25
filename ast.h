#ifndef AST_H
#define AST_H

#include "tokens.h"
#include "vector.h"
#include <stdbool.h>

enum ast_node_type {
  FN_DEF_STMT,
  VARIABLE_DECL_STMT,
  VARIABLE_ASSIGN_STMT,
  IF_STMT,
  WHILE_STMT,
  BLOCK_STMT,
  BINARY_NODE,
  PRIMARY_NODE
};

enum ast_primary_node_type {
  NUMBER_PRIMARY_NODE,
  STRING_PRIMARY_NODE,
  IDENTIFIER_PRIMARY_NODE,
  BOOLEAN_PRIMARY_NODE,
  NIL_PRIMARY_NODE
};

struct ast_fn_def_parameter {
  enum token_type parameter_type;
  char *parameter_name;
};

struct ast_node {
  enum ast_node_type node_type;
  enum ast_primary_node_type primary_node_type;

  /* Function definition statement */
  enum token_type fn_def_stmt_return_type;
  struct ast_node *fn_def_stmt_id;
  struct vector
      *fn_def_stmt_parameters; /* Vector of `ast_fn_def_parameter'. The arity is
                                  implied in vector `size'. */
  struct ast_node *fn_def_stmt_block;

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
