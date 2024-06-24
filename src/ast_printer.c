#include "ast_printer.h"
#include "ast.h"
#include "string_builder.h"
#include "tokens.h"

struct string_builder *print_ast(struct vector *program) {
  struct string_builder *str = string_builder_init();
  for (size_t i = 0; i < program->size; i++) {
    print_statement(vector_at(program, i), str, 0);
  }
  return str;
}

void print_statement(struct ast_node *node, struct string_builder *str,
                     size_t indent_level) {
  switch (node->node_type) {
  case FN_DEF_STMT: {
    print_fn_def_stmt(node, str, indent_level);
    break;
  }
  case EXPR_STMT: {
    print_expr_stmt(node, str, indent_level);
    break;
  }
  case RETURN_STMT: {
    print_return_stmt(node, str, indent_level);
    break;
  }
  case VARIABLE_DECL_STMT: {
    print_var_decl_stmt(node, str, indent_level);
    break;
  }
  case VARIABLE_ASSIGN_STMT: {
    print_var_assign_stmt(node, str, indent_level);
    break;
  }
  case IF_STMT: {
    print_if_stmt(node, str, indent_level);
    break;
  }
  case WHILE_STMT: {
    print_while_stmt(node, str, indent_level);
    break;
  }
  case FOR_STMT: {
    print_for_stmt(node, str, indent_level);
    break;
  }
  case BREAK_STMT: {
    print_break_stmt(node, str, indent_level);
    break;
  }
  case BLOCK_STMT: {
    print_block_stmt(node, str, indent_level);
    break;
  }
  default: {
    printf("Invalid statement type.\n");
    exit(1);
  }
  }
}

void print_fn_def_stmt(struct ast_node *node, struct string_builder *str,
                       size_t indent_level) {
  string_builder_append(str, get_indent_str(indent_level));
  string_builder_append(str, "FunctionDefinitionStatement: ");
  string_builder_append(str, node->fn_def_stmt.id);
  string_builder_append(str, " ( ");
  for (size_t i = 0; i < node->fn_def_stmt.parameters->size; i++) {
    string_builder_append(str, vector_at(node->fn_def_stmt.parameters, i));
    if (i + 1 != node->fn_def_stmt.parameters->size) {
      string_builder_append(str, ", ");
    }
  }
  string_builder_append(str, " )\n");
  print_statement(node->fn_def_stmt.block, str, indent_level + 1);
}

void print_expr_stmt(struct ast_node *node, struct string_builder *str,
                     size_t indent_level) {
  string_builder_append(str, get_indent_str(indent_level));
  string_builder_append(str, "ExpressionStatement: ");
  print_expression(node->expr_stmt_expr, str);
  string_builder_append(str, "\n");
}

void print_return_stmt(struct ast_node *node, struct string_builder *str,
                       size_t indent_level) {
  string_builder_append(str, get_indent_str(indent_level));
  string_builder_append(str, "ReturnStatement: ");
  print_expression(node->expr_stmt_expr, str);
  string_builder_append(str, "\n");
}

void print_var_decl_stmt(struct ast_node *node, struct string_builder *str,
                         size_t indent_level) {
  string_builder_append(str, get_indent_str(indent_level));
  string_builder_append(str, "VariableDeclStatement: ");
  string_builder_append(str, node->var_decl_stmt.id);
  string_builder_append(str, " = ");
  print_expression(node->var_decl_stmt.expr, str);
  string_builder_append(str, "\n");
}

void print_var_assign_stmt(struct ast_node *node, struct string_builder *str,
                           size_t indent_level) {
  string_builder_append(str, get_indent_str(indent_level));
  string_builder_append(str, "VariableAssignStatement: ");
  print_primary_expression(node->var_assign_stmt.primary, str);
  string_builder_append(str, " = ");
  print_expression(node->var_assign_stmt.expr, str);
  string_builder_append(str, "\n");
}

void print_if_stmt(struct ast_node *node, struct string_builder *str,
                   size_t indent_level) {
  string_builder_append(str, get_indent_str(indent_level));
  string_builder_append(str, "IfStatement: ");
  print_expression(node->if_else_stmt.expr, str);
  string_builder_append(str, "\n");
  print_statement(node->if_else_stmt.if_block, str, indent_level + 1);
  if (node->if_else_stmt.else_block) {
    string_builder_append(str, get_indent_str(indent_level));
    string_builder_append(str, "ElseStatement: \n");
    print_statement(node->if_else_stmt.else_block, str, indent_level + 1);
  }
}

void print_while_stmt(struct ast_node *node, struct string_builder *str,
                      size_t indent_level) {
  string_builder_append(str, get_indent_str(indent_level));
  string_builder_append(str, "WhileStatement: ");
  print_expression(node->while_stmt.expr, str);
  string_builder_append(str, "\n");
  print_statement(node->while_stmt.block, str, indent_level + 1);
}

void print_for_stmt(struct ast_node *node, struct string_builder *str,
                    size_t indent_level) {
  string_builder_append(str, get_indent_str(indent_level));
  string_builder_append(str, "ForStatement: \n");
  print_statement(node->for_stmt.init_stmt, str, indent_level + 1);
  print_statement(node->for_stmt.expr_stmt, str, indent_level + 1);
  print_statement(node->for_stmt.update_stmt, str, indent_level + 1);
  print_statement(node->for_stmt.block, str, indent_level + 2);
}

void print_break_stmt(struct ast_node *node, struct string_builder *str,
                      size_t indent_level) {
  string_builder_append(str, get_indent_str(indent_level));
  string_builder_append(str, "BreakStatement\n");
}

void print_block_stmt(struct ast_node *node, struct string_builder *str,
                      size_t indent_level) {
  string_builder_append(str, get_indent_str(indent_level));
  string_builder_append(str, "BlockStatement: \n");
  for (size_t i = 0; i < node->block_stmt_stmts->size; i++) {
    print_statement(vector_at(node->block_stmt_stmts, i), str,
                    indent_level + 1);
  }
}

void print_expression(struct ast_node *node, struct string_builder *str) {
  switch (node->node_type) {
  case BINARY_NODE: {
    print_binary_expression(node, str);
    break;
  }
  case UNARY_NODE: {
    print_unary_expression(node, str);
    break;
  }
  case PRIMARY_NODE: {
    print_primary_expression(node, str);
    break;
  }
  default: {
    printf("Invalid expression node type.\n");
    exit(1);
  }
  }
}

void print_binary_expression(struct ast_node *node,
                             struct string_builder *str) {
  string_builder_append(str, "( ");
  print_expression(node->binary.left, str);
  string_builder_append(str, " ");
  string_builder_append(str, get_string_from_token_atom(node->binary.op));
  string_builder_append(str, " ");
  print_expression(node->binary.right, str);
  string_builder_append(str, " ) ");
}

void print_unary_expression(struct ast_node *node, struct string_builder *str) {
  string_builder_append(str, get_string_from_token_atom(node->unary.op));
  string_builder_append(str, " ");
  print_primary_expression(node->unary.primary, str);
}

void print_primary_expression(struct ast_node *node,
                              struct string_builder *str) {
  switch (node->primary_node_type) {
  case NUMBER_PRIMARY_NODE: {
    string_builder_append(str, number_to_char(node->number));
    break;
  }
  case STRING_PRIMARY_NODE: {
    string_builder_append(str, "\"");
    string_builder_append(str, escape_special_characters(node->string));
    string_builder_append(str, "\"");
    break;
  }
  case IDENTIFIER_PRIMARY_NODE: {
    string_builder_append(str, node->id);
    break;
  }
  case BOOLEAN_PRIMARY_NODE: {
    if (node->boolean) {
      string_builder_append(str, "True");
    } else {
      string_builder_append(str, "False");
    }
    break;
  }
  case NIL_PRIMARY_NODE: {
    string_builder_append(str, "Nil");
    break;
  }
  case FN_CALL_PRIMARY_NODE: {
    print_primary_expression(node->fn_call.primary, str);
    string_builder_append(str, " ( ");
    for (size_t i = 0; i < node->fn_call.parameters->size; i++) {
      print_expression(vector_at(node->fn_call.parameters, i), str);
      if (i + 1 != node->fn_call.parameters->size) {
        string_builder_append(str, ", ");
      }
    }
    string_builder_append(str, " ) ");
    break;
  }
  case METHOD_CALL_PRIMARY_NODE: {
    print_primary_expression(node->method_call.object, str);
    string_builder_append(str, ".");
    print_primary_expression(node->method_call.member, str);
    break;
  }
  case ARRAY_CREATION_PRIMARY_NODE: {
    string_builder_append(str, "[ ");
    for (size_t i = 0; i < node->array->size; i++) {
      print_expression(vector_at(node->array, i), str);
      if (i + 1 != node->array->size) {
        string_builder_append(str, ", ");
      }
    }
    string_builder_append(str, " ] ");
    break;
  }
  case ARRAY_ACCESS_PRIMARY_NODE: {
    print_expression(node->array_access.primary, str);
    string_builder_append(str, " [ ");
    print_expression(node->array_access.index, str);
    string_builder_append(str, " ] ");
    break;
  }
  }
}

char *get_indent_str(size_t indent_level) {
  if (indent_level > 0) {
    char *buffer = malloc(indent_level + 1);
    memset(buffer, '\t', indent_level);
    buffer[indent_level + 1] = 0;
    return buffer;
  }
  return "";
}

char *escape_special_characters(const char *input) {
  size_t len = strlen(input);
  size_t new_len = len * 2 + 1; /* Estimated worst case new string length */
  char *escaped_str = malloc(new_len);
  size_t j = 0;
  for (size_t i = 0; i < len; i++) {
    switch (input[i]) {
    case '\n': {
      escaped_str[j++] = '\\';
      escaped_str[j++] = 'n';
      break;
    }
    case '\t': {
      escaped_str[j++] = '\\';
      escaped_str[j++] = 't';
      break;
    }
    case '\\': {
      escaped_str[j++] = '\\';
      escaped_str[j++] = '\\';
      break;
    }
    case '\"': {
      escaped_str[j++] = '\\';
      escaped_str[j++] = '\"';
      break;
    }
    default: {
      escaped_str[j++] = input[i];
      break;
    }
    }
  }
  escaped_str[j] = 0;
  return escaped_str;
}
