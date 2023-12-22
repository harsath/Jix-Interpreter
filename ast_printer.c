#include "ast_printer.h"
#include "ast.h"

void print_ast_program_var_decl_stmt(ast_node *var_decl_stmt) {
  printf("%s\n",
         get_string_from_token_atom(var_decl_stmt->var_decl_stmt_dtype));
  print_ast(var_decl_stmt->var_decl_stmt_id, 0);
  printf("=\n");
  print_ast(var_decl_stmt->var_decl_stmt_expr, 0);
}

void print_ast_program_var_assign_stmt(ast_node *var_assign_stmt) {
  print_ast(var_assign_stmt->assign_stmt_id, 0);
  printf("=\n");
  print_ast(var_assign_stmt->assign_stmt_expr, 0);
}

void print_ast_program(vector *program) {
  for (size_t i = 0; i < program->size; i++) {
    ast_node *statement = vector_at(program, i);
    switch (statement->node_type) {
    case VARIABLE_DECL_STMT: {
      print_ast_program_var_decl_stmt(statement);
      break;
    }
    case VARIABLE_ASSIGN_STMT: {
print_ast_program_var_assign_stmt(statement);
break;
                               }
    default: {
      printf("Unsupported statement type on print_ast_program\n");
    }
    }
  }
}

void print_ast(ast_node *node, size_t level) {
  if (node == NULL) {
    return;
  }
  for (size_t i = 0; i < level; i++) {
    printf(i == level - 1 ? "| " : "  ");
  }
  switch (node->node_type) {
  case BINARY_NODE: {
    printf("%s\n", get_string_from_token_atom(node->op));
    print_ast(node->left, level + 1);
    print_ast(node->right, level + 1);
    break;
  }

  case PRIMARY_NODE: {
    switch (node->primary_node_type) {
    case NUMBER_PRIMARY_NODE: {
      printf("%ld", node->number_value);
      printf("\n");
      break;
    }
    case STRING_PRIMARY_NODE: {
      printf("\"%s\"", node->string_value);
      printf("\n");
      break;
    }
    case IDENTIFIER_PRIMARY_NODE: {
      printf("%s", node->identifier_value);
      printf("\n");
      break;
    }
    case BOOLEAN_PRIMARY_NODE: {
      printf("%s\n", node->boolean_value ? "true" : "false");
      break;
    }
    case NIL_PRIMARY_NODE: {
      printf("nil\n");
      break;
    }
    }
    break;
  }
  default:
    printf("Invalid node type in print_ast\n");
  }
}
