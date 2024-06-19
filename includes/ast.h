#ifndef AST_H
#define AST_H

#include "tokens.h"
#include "vector.h"
#include <stdbool.h>

enum ast_node_type {
  FN_DEF_STMT,
  EXPR_STMT,
  RETURN_STMT,
  VARIABLE_DECL_STMT,
  VARIABLE_ASSIGN_STMT,
  IF_STMT,
  WHILE_STMT,
  FOR_STMT,
  BREAK_STMT,
  BLOCK_STMT,
  BINARY_NODE,
  UNARY_NODE,
  PRIMARY_NODE,
};

enum ast_primary_node_type {
  NUMBER_PRIMARY_NODE,
  STRING_PRIMARY_NODE,
  IDENTIFIER_PRIMARY_NODE,
  BOOLEAN_PRIMARY_NODE,
  NIL_PRIMARY_NODE,
  FN_CALL_PRIMARY_NODE,
  ARRAY_CREATION_PRIMARY_NODE,
  ARRAY_ACCESS_PRIMARY_NODE
};

struct ast_node {
  enum ast_node_type node_type;
  enum ast_primary_node_type primary_node_type;

  union {
    /* Function definition statement */
    struct {
      char *id;
      struct vector *parameters; /* Vector of `char*` */
      struct ast_node *block;
    } fn_def_stmt;

    /* Variable declaration statement */
    struct {
      char *id;
      struct ast_node *expr;
    } var_decl_stmt;

    /* Variable assignment statement */
    struct {
      char *id;
      struct ast_node *expr;
    } var_assign_stmt;

    /* If-else statement */
    struct {
      struct ast_node *expr;
      struct ast_node *if_block;
      struct ast_node *else_block;
    } if_else_stmt;

    /* While statement */
    struct {
      struct ast_node *expr;
      struct ast_node *block;
    } while_stmt;

    /* For statement */
    struct {
      struct ast_node *init_stmt;
      struct ast_node *expr_stmt;
      struct ast_node *update_stmt;
      struct ast_node *block;
    } for_stmt;

    /* Return statement */
    struct ast_node *return_stmt_expr;

    /* Block statement */
    struct vector *block_stmt_stmts;

    /* Expression statement */
    struct ast_node *expr_stmt_expr;

    /* Binary node */
    struct {
      struct ast_node *left;
      struct ast_node *right;
      enum token_type op;
    } binary;

    /* Unary node */
    struct {
      enum token_type op;
      struct ast_node *primary;
    } unary;

    /* Number node */
    long number;

    /* String node */
    char *string;

    /* Identifier node */
    char *id;

    /* Boolean node*/
    bool boolean;

    /* Function call */
    struct {
      char *id;
      struct vector *parameters; /* Vector of `ast_node` of expressions */
    } fn_call;

    /* Array node */
    struct vector *array; /* Vector of `ast_node` of expressions */

    /* Array access node */
    struct {
      struct ast_node *primary; /* One of 3 from grammar */
      struct ast_node *index;
    } array_access;
  };
};

#endif
