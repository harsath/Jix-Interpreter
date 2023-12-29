#include "ast_printer.h"
#include "ast.h"
#include "tokens.h"
#include "vector.h"

void print_ast_program_fn_def_stmt(struct ast_node *fn_def_stmt,
                                   size_t indent_level) {
  printf("fn %s ( ", fn_def_stmt->fn_def_stmt_id->identifier_value);
  for (size_t i = 0; i < fn_def_stmt->fn_def_stmt_parameters->size; i++) {
    char *parameter = vector_at(fn_def_stmt->fn_def_stmt_parameters, i);
    printf("%s", parameter);
    if (fn_def_stmt->fn_def_stmt_parameters->size > (i + 1)) {
      printf(", ");
    }
  }
  printf(" ) ");
  print_ast_program_block_stmt(fn_def_stmt->fn_def_stmt_block, indent_level);
  printf("\n");
}

void print_ast_program_expr_stmt(struct ast_node *expr_stmt) {
  print_ast_expr(expr_stmt->expr_stmt_expr);
  printf("\n");
}

void print_ast_program_return_stmt(struct ast_node *return_stmt) {
  printf("return ");
  print_ast_expr(return_stmt->return_stmt_expr);
  printf("\n");
}

void print_ast_program_var_decl_stmt(struct ast_node *var_decl_stmt) {
  printf("let ");
  print_ast_expr(var_decl_stmt->var_decl_stmt_id);
  printf(" = ");
  print_ast_expr(var_decl_stmt->var_decl_stmt_expr);
  printf("\n");
}

void print_ast_program_var_assign_stmt(struct ast_node *var_assign_stmt) {
  print_ast_expr(var_assign_stmt->assign_stmt_id);
  printf(" = ");
  print_ast_expr(var_assign_stmt->assign_stmt_expr);
  printf("\n");
}

void print_ast_program_if_stmt(struct ast_node *if_stmt, size_t indent_level) {
  printf("if ( ");
  print_ast_expr(if_stmt->if_stmt_expr);
  printf(" ) ");
  print_ast_program_block_stmt(if_stmt->if_stmt_block, indent_level);
  if (if_stmt->if_else_stmt_block != NULL) {
    printf(" else ");
    print_ast_program_block_stmt(if_stmt->if_else_stmt_block, indent_level);
  }
  printf("\n");
}

void print_ast_program_while_stmt(struct ast_node *while_stmt,
                                  size_t indent_level) {
  printf("while ( ");
  print_ast_expr(while_stmt->while_stmt_expr);
  printf(" ) ");
  print_ast_program_block_stmt(while_stmt->while_stmt_block, indent_level);
  printf("\n");
}

void print_ast_program_break_stmt(struct ast_node *break_stmt,
                                  size_t indent_level) {
  printf("break ;\n");
}

void print_ast_program_block_stmt(struct ast_node *block_stmt,
                                  size_t indent_level) {
  printf("{ ");
  for (size_t i = 0; i < block_stmt->block_stmt_stmts->size; i++) {
    printf("\n");
    print_ast_statement(vector_at(block_stmt->block_stmt_stmts, i),
                        indent_level + 1);
  }
  for (size_t i = 0; i < indent_level; i++) {
    printf("\t");
  }
  printf("}");
}

void print_ast_program(struct vector *program) {
  for (size_t i = 0; i < program->size; i++) {
    print_ast_statement(vector_at(program, i), 0);
    printf("----------------------------------------------------------\n");
  }
}

void print_ast_statement(struct ast_node *statement, size_t indent_level) {
  for (size_t i = 0; i < indent_level; i++) {
    printf("\t");
  }
  switch (statement->node_type) {
  case FN_DEF_STMT: {
    print_ast_program_fn_def_stmt(statement, indent_level);
    break;
  }
  case EXPR_STMT: {
    print_ast_program_expr_stmt(statement);
    break;
  }
  case RETURN_STMT: {
    print_ast_program_return_stmt(statement);
    break;
  }
  case VARIABLE_DECL_STMT: {
    print_ast_program_var_decl_stmt(statement);
    break;
  }
  case VARIABLE_ASSIGN_STMT: {
    print_ast_program_var_assign_stmt(statement);
    break;
  }
  case BLOCK_STMT: {
    print_ast_program_block_stmt(statement, indent_level);
    break;
  }
  case IF_STMT: {
    print_ast_program_if_stmt(statement, indent_level);
    break;
  }
  case WHILE_STMT: {
    print_ast_program_while_stmt(statement, indent_level);
    break;
  }
  case BREAK_STMT: {
    print_ast_program_break_stmt(statement, indent_level);
    break;
  }
  default: {
    printf("Unsupported statement type on print_ast_program\n");
  }
  }
}

void print_ast_expr(struct ast_node *node) {
  if (node == NULL) {
    return;
  }
  switch (node->node_type) {
  case FN_CALL_NODE: {
    print_ast_expr(node->fn_call_identifier);
    printf(" ( ");
    for (size_t i = 0; i < node->fn_call_parameters->size; i++) {
      print_ast_expr(vector_at(node->fn_call_parameters, i));
      if ((i + 1) != node->fn_call_parameters->size) {
        printf(", ");
      }
    }
    printf(" ) ");
    break;
  }

  case BINARY_NODE: {
    printf(" ( ");
    print_ast_expr(node->left);
    printf(" %s ", get_string_from_token_atom(node->op));
    print_ast_expr(node->right);
    printf(" ) ");
    break;
  }

  case PRIMARY_NODE: {
    switch (node->primary_node_type) {
    case NUMBER_PRIMARY_NODE: {
      printf("%ld", node->number_value);
      break;
    }
    case STRING_PRIMARY_NODE: {
      printf("\"%s\"", node->string_value);
      break;
    }
    case IDENTIFIER_PRIMARY_NODE: {
      printf("%s", node->identifier_value);
      break;
    }
    case BOOLEAN_PRIMARY_NODE: {
      printf("%s", node->boolean_value ? "true" : "false");
      break;
    }
    case NIL_PRIMARY_NODE: {
      printf("nil");
      break;
    }
    }
    break;
  }
  default:
    printf("Invalid node type in print_ast\n");
  }
}
