#include "utils.h"
#include "ast_printer.h"
#include "interpreter.h"
#include "parser.h"
#include "scanner.h"
#include "string_builder.h"
#include "tokens.h"
#include "vector.h"
#include <stdio.h>

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

void print_ast_pipeline(const char *file_name) {
  char *input = read_file(file_name);
  if (!input) {
    return;
  }
  struct vector *tokens = scan_tokens(input);
  struct vector *program = parse_program(tokens);
  printf("%s", print_ast(program)->str);
}

const char *convert_object_to_string(struct object *obj) {
  switch (obj->data_type) {
  case INT_VALUE: {
    char *buffer = malloc(20);
    sprintf(buffer, "%ld", obj->int_value);
    return buffer;
  }
  case BOOLEAN_VALUE: {
    return obj->bool_value ? "true" : "false";
  }
  case FUNCTION_VALUE: {
    return "<function>";
  }
  case NIL_VALUE: {
    return "nil";
  }
  case ARRAY_VALUE: {
    struct string_builder *str_builder = string_builder_init();
    string_builder_append(str_builder, "[");
    for (size_t i = 0; i < obj->array_value->size; i++) {
      struct object *val = vector_at(obj->array_value, i);
      if (val->data_type == STRING_VALUE) {
        string_builder_append(str_builder, "\"");
        string_builder_append(str_builder, convert_object_to_string(
                                             vector_at(obj->array_value, i)));
        string_builder_append(str_builder, "\"");
      } else {
        string_builder_append(str_builder, convert_object_to_string(
                                             vector_at(obj->array_value, i)));
      }
      if (i != obj->array_value->size - 1) {
        string_builder_append(str_builder, ", ");
      }
    }
    string_builder_append(str_builder, "]");
    return str_builder->str;
  }
  case STRING_VALUE: {
    return obj->string_value;
  }
  default: {
    printf("Unsupported type to convert as string.\n");
    exit(1);
  }
  }
}
