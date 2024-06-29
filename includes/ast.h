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
  METHOD_CALL_PRIMARY_NODE,
  ARRAY_CREATION_PRIMARY_NODE,
  ARRAY_ACCESS_PRIMARY_NODE,
};

struct ast_node {
  enum ast_node_type node_type;
  enum ast_primary_node_type primary_node_type;

  union {
    /* Function definition statement */
    struct {
      char *id;
      struct vector *parameters; /* Vector of result(`char*`) */
      struct result *block;
    } fn_def_stmt;

    /* Variable declaration statement */
    struct {
      char *id;
      struct result *expr;
    } var_decl_stmt;

    /* Variable assignment statement */
    struct {
      struct result *primary;
      struct result *expr;
    } var_assign_stmt;

    /* If-else statement */
    struct {
      struct result *expr;
      struct result *if_block;
      struct result *else_block;
    } if_else_stmt;

    /* While statement */
    struct {
      struct result *expr;
      struct result *block;
    } while_stmt;

    /* For statement */
    struct {
      struct result *init_stmt;
      struct result *expr_stmt;
      struct result *update_stmt;
      struct result *block;
    } for_stmt;

    /* Return statement */
    struct result *return_stmt_expr;

    /* Block statement */
    struct vector *block_stmt_stmts;

    /* Expression statement */
    struct result *expr_stmt_expr;

    /* Binary node */
    struct {
      struct result *left;
      struct result *right;
      enum token_type op;
    } binary;

    /* Unary node */
    struct {
      enum token_type op;
      struct result *primary;
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
      struct result *primary;
      struct vector
          *parameters; /* Vector of result(`ast_node`) of expressions */
    } fn_call;

    /* Method call */
    struct {
      struct result *object;
      struct result *member; /* Function call */
    } method_call;

    /* Array node */
    struct vector *array; /* Vector of result(`ast_node`) of expressions */

    /* Array access node */
    struct {
      struct result *primary; /* One of 3 from grammar */
      struct result *index;
    } array_access;
  };
};

const char *get_string_from_ast_node_type(enum ast_node_type node_type);
const char *
get_string_from_primary_ast_node_type(enum ast_primary_node_type node_type);

#endif
