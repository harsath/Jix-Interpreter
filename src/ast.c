#include "ast.h"

const char *get_string_from_ast_node_type(enum ast_node_type node_type) {
  switch (node_type) {
  case FN_DEF_STMT:
    return "Function definition statement";
  case EXPR_STMT:
    return "Expression statement";
  case RETURN_STMT:
    return "Return statement";
  case VARIABLE_DECL_STMT:
    return "Variable declaration statement";
  case VARIABLE_ASSIGN_STMT:
    return "Variable assignment statement";
  case IF_STMT:
    return "If statement";
  case WHILE_STMT:
    return "While statement";
  case FOR_STMT:
    return "For statement";
  case BREAK_STMT:
    return "Break statement";
  case BLOCK_STMT:
    return "Block statement";
  case BINARY_NODE:
    return "Binary expression";
  case UNARY_NODE:
    return "Unary expression";
  case PRIMARY_NODE:
    return "Primary expression";
  }
}

const char *
get_string_from_primary_ast_node_type(enum ast_primary_node_type node_type) {
  switch (node_type) {
  case NUMBER_PRIMARY_NODE:
    return "Number";
  case STRING_PRIMARY_NODE:
    return "String";
  case IDENTIFIER_PRIMARY_NODE:
    return "Identifier";
  case BOOLEAN_PRIMARY_NODE:
    return "Boolean";
  case NIL_PRIMARY_NODE:
    return "Nil";
  case FN_CALL_PRIMARY_NODE:
    return "Function call";
  case METHOD_CALL_PRIMARY_NODE:
    return "Method call";
  case ARRAY_CREATION_PRIMARY_NODE:
    return "Array creation";
  case ARRAY_ACCESS_PRIMARY_NODE:
    return "Array creation";
  }
}
