#include "utils.h"
#include "ast_printer.h"
#include "errors.h"
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
  struct parser *program = parse_program(tokens);
  if (program->parser_errors) {
    exit(1);
  }
  struct object *interpreter_return_value = interpret(program->program);
  vector_free(tokens);
  return interpreter_return_value;
}

void print_ast_pipeline(const char *file_name) {
  char *input = read_file(file_name);
  if (!input) {
    return;
  }
  struct vector *tokens = scan_tokens(input);
  struct parser *program = parse_program(tokens);
  if (program->parser_errors) {
    printf("fix parser errors first\n");
    exit(1);
  }
  printf("%s", print_ast(program->program)->str);
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

// https://stackoverflow.com/a/1056423
char *format_string(const char *format, ...) {
  va_list args;
  va_list args_copy;
  int length;
  char *buffer;

  va_start(args, format);
  va_copy(args_copy, args);
  length = vsnprintf(NULL, 0, format, args_copy);
  va_end(args_copy);
  buffer = malloc(length + 1);
  if (buffer == NULL) {
    va_end(args);
    return NULL;
  }
  vsnprintf(buffer, length + 1, format, args);
  va_end(args);
  return buffer;
}

void print_parser_errors(struct vector *parser_errors) {
  for (size_t i = 0; i < parser_errors->size; i++) {
    struct parser_error *err = vector_at(parser_errors, i);
    char *error_message =
        format_string("Syntax Error (line %li): %s\n", err->line, err->message);
    printf("%s", error_message);
  }
}

void print_interpreter_error(struct runtime_error *error) {
  char *error_message;
  if (error->start_line != error->end_line) {
    error_message =
        format_string("Runtime Error (lines %li-%li): %s\n", error->start_line,
                      error->end_line, error->message);
  } else {
    error_message = format_string("Runtime Error (line %li): %s\n",
                                  error->start_line, error->message);
  }
  printf("%s", error_message);
}
