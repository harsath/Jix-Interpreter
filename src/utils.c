#include "utils.h"
#include "scanner.h"
#include "parser.h"
#include "tokens.h"
#include "interpreter.h"

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
                               size_t end_index) {
  char *iden_buffer = calloc(IDENTIFIER_BUFFER_SIZE, sizeof(char));
  memcpy(iden_buffer, (char_ptr + start_index), (end_index - start_index));
  return iden_buffer;
}

struct object *interpreter_pipeline(const char *file_name) {
  char *input = read_file(file_name);
  if (!input) {
    return NULL;
  }
  struct vector *tokens = scan_tokens(input);
  struct vector *program = parse_program(tokens);
  struct object *interpreter_return_value = interpret(program);
  vector_free(tokens);
  return interpreter_return_value;
}