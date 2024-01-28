#include "ast_printer.h"
#include "interpreter.h"
#include "parser.h"
#include "scanner.h"
#include "tokens.h"
#include "utils.h"

int main(int argc, const char *argv[]) {
  if (argc < 2) {
    printf("Usage: ./lox [script]\n");
    return -1;
  }
  const char *file_name = argv[1];
  char *input = read_file(file_name);
  if (!input) {
    return -1;
  }
  struct vector *tokens = scan_tokens(input);

  struct vector *program = parse_program(tokens);

  print_ast_program(program);

  struct object *interpreter_value = interpret(program);

  if (!interpreter_value) {
    printf("Interpreter doesn't return a value.\n");
  } else {
    printf("Resultx: %li\n", interpreter_value->int_value);
    /* printf("Resultx: %s\n", interpreter_value->string_value); */
  }

  vector_free(tokens);

  return 0;
}
