#ifndef utils_h
#define utils_h

#include "ast.h"
#include "tokens.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 10240
#define IDENTIFIER_BUFFER_SIZE 50

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
    printf("%ld\n", node->number_value);
    break;
  }
  }
}

#endif
