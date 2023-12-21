#include "utils.h"

char *read_file(const char *file_path) {
  if (!file_path) {
    perror("Filepath is NULL");
    return NULL;
  }
  int fd = open(file_path, O_RDONLY);
  if (fd < 0) {
    perror("Error opening file");
    return NULL;
  }

  char *buffer = calloc(BUFFER_SIZE, sizeof(char));
  if (!buffer) {
    perror("Memory allocation error on for input file buffer");
    close(fd);
    return NULL;
  }

  ssize_t bytes_read = read(fd, buffer, BUFFER_SIZE - 1);
  if (buffer < 0) {
    perror("Error reading file");
    close(fd);
    free(buffer);
    return NULL;
  }

  close(fd);

  return buffer;
}

char *create_token_string_copy(const char *char_ptr, size_t start_index,
                               size_t current_index) {
  char *iden_buffer = calloc(IDENTIFIER_BUFFER_SIZE, sizeof(char));
  memcpy(iden_buffer, (char_ptr + start_index), (current_index - start_index));
  return iden_buffer;
}

void print_ast_program_var_decl_stmt(ast_node *var_decl_stmt) {
  printf("%s\n",
         get_string_from_token_atom(var_decl_stmt->var_decl_stmt_dtype));
  print_ast(var_decl_stmt->var_decl_stmt_id, 0);
  printf("=\n");
  print_ast(var_decl_stmt->var_decl_stmt_expr, 0);
}

void print_ast_program(vector *program) {
  for (size_t i = 0; i < program->size; i++) {
    ast_node *statement = vector_at(program, i);
    switch (statement->node_type) {
    case VARIABLE_DECL_STMT: {
      print_ast_program_var_decl_stmt(statement);
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
      printf("\"%.*s\"", (int)node->string_value->token_char_len,
             node->string_value->token_char);
      printf("\n");
      break;
    }
    case IDENTIFIER_PRIMARY_NODE: {
      printf("%.*s", (int)node->identifier_value->token_char_len,
             node->identifier_value->token_char);
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
