#include "interpreter.h"

long interpret(ast_node *ast) {
  if (!ast) {
    return -1;
  }
  interpreter_state state = { .variables = hash_table_init() };
  return interpret_ast(ast, &state);
}

long interpret_ast(ast_node *ast, interpreter_state *state) {
  switch (ast->node_type) {
  case VARIABLE_DECL_STMT: {
    char temp_identifier[100] = {0};
    ast_node *identifier = ast->var_decl_stmt_id;
    memcpy(temp_identifier, identifier->identifier_value->token_char, identifier->identifier_value->token_char_len);
  }
  case BINARY_NODE: {
    long lhs = interpret_ast(ast->left, state);
    long rhs = interpret_ast(ast->right,state);
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
    case IDENTIFIER_PRIMARY_NODE: {

      printf("Identifier primary node unimplemented\n");
      exit(1);
    }
    case NIL_PRIMARY_NODE: {

      printf("Nil primary node unimplemented\n");
      exit(1);
    }
    }
  }
  }
}
