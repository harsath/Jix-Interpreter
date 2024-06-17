#include "interpreter.h"
#include "parser.h"
#include "scanner.h"
#include "tokens.h"
#include "utils.h"

int main(int argc, const char *argv[]) {
  if (argc < 2) {
    printf("Usage: ./jix [script]\n");
    return -1;
  }
  const char *file_name = argv[1];
  // print_ast_program(program);

 print_ast_pipeline(file_name); 
  /*
  struct object *interpreter_value = interpreter_pipeline(file_name);

  if (!interpreter_value) {
    printf("Interpreter doesn't return a value.\n");
  } else {
    // Interpreter can also return string_value or bool_value
    printf("Return: %li\n", interpreter_value->int_value);
  }
  */

  return 0;
}
