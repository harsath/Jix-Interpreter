#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "ast.h"
#include "tokens.h"

long interpret_ast(ast_node *ast);

long interpret(ast_node *ast) {
  if (!ast) {
    return -1;
  }
  return interpret_ast(ast);
}

long interpret_ast(ast_node *ast) {
  switch (ast->node_type) {
  case BINARY_NODE: {
    long lhs = interpret_ast(ast->left);
    long rhs = interpret_ast(ast->right);
    switch (ast->op) {
    case PLUS: {
      return lhs + rhs;
    }
    case MINUS: {
      return lhs - rhs;
    }
    case STAR: {
      return lhs * rhs;
    }
    case SLASH: {
      return lhs / rhs;
    }
    default:
      printf("Invalid operation '%s' in binary node\n",
             get_string_from_token_atom(ast->op));
      exit(1);
    }
  }
  case PRIMARY_NODE: {
    switch (ast->primary_node_type) {
    case NUMBER_PRIMARY_NODE: {
      return ast->number_value;
    }
    case STRING_PRIMARY_NODE: {
      printf("String primary node unimplemented\n");
      exit(1);
    }
    case BOOLEAN_PRIMARY_NODE: {
      printf("Boolean primary node unimplemented\n");
      exit(1);
    }
    }
  }
  }
}

#endif
