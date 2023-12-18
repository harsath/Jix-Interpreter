#include "utils.h"
#include "scanner.h"
#include "tokens.h"

int main(int argc, const char *argv[]){
	const char *file_name = "foo.jix";
	char *input = read_file(file_name);
	if (!input) {
		return -1;
	}
	vector *tokens = scan_tokens(input);

	for (size_t i = 0; i < tokens->size; i++) {
		token *tok = (token*)tokens->_internal_buffer[i];
		printf("type: %s, value: '%.*s'\n", get_string_from_token_atom(tok->type), (int)tok->token_char_len, tok->token_char);
	}

	vector_free(tokens);
	
	return 0;
}
