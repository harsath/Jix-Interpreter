#ifndef scanner_h
#define scanner_h

#include <stdlib.h>
#include <ctype.h>
#include "tokens.h"
#include "utils.h"
#include "vector.h"

vector *scan_tokens(const char *source_code) {
	if (!source_code) { return NULL; }
	vector *tokens = vector_init();
	size_t current_index = 0;
	size_t start_index = 0;
	size_t source_code_len = strlen(source_code);
	while (current_index != source_code_len) {
		if (isdigit(source_code[current_index])) {
			current_index++;
			while (isdigit(source_code[current_index])) {
				current_index++;
			}
			token *token_ = create_token(NUMBER, (source_code + start_index), 
					(current_index - start_index));
			vector_push_back(tokens, token_);
		} else if(source_code[current_index] == '(') {
			current_index++;
			token *token_ = create_token(LEFT_PAREN, (source_code + start_index), 
					(current_index - start_index));
			vector_push_back(tokens, token_);
		} else if(source_code[current_index] == ')') {
			current_index++;
			token *token_ = create_token(RIGHT_PAREN, (source_code + start_index), 
					(current_index - start_index));
			vector_push_back(tokens, token_);
		} else if(source_code[current_index] == '{') {
			current_index++;
			token *token_ = create_token(LEFT_BRACE, (source_code + start_index), 
					(current_index - start_index));
			vector_push_back(tokens, token_);
		} else if(source_code[current_index] == '}') {
			current_index++;
			token *token_ = create_token(RIGHT_PAREN, (source_code + start_index), 
					(current_index - start_index));
			vector_push_back(tokens, token_);
		} else if(source_code[current_index] == ',') {
			current_index++;
			token *token_ = create_token(COMMA, (source_code + start_index), 
					(current_index - start_index));
			vector_push_back(tokens, token_);
		} else if(source_code[current_index] == '.') {
			current_index++;
			token *token_ = create_token(DOT, (source_code + start_index), 
					(current_index - start_index));
			vector_push_back(tokens, token_);
		} else if(source_code[current_index] == '-') {
			current_index++;
			token *token_ = create_token(MINUS, (source_code + start_index), 
					(current_index - start_index));
			vector_push_back(tokens, token_);
		} else if(source_code[current_index] == '+') {
			current_index++;
			token *token_ = create_token(PLUS, (source_code + start_index), 
					(current_index - start_index));
			vector_push_back(tokens, token_);
		} else if(source_code[current_index] == ';') {
			current_index++;
			token *token_ = create_token(SEMICOLON, (source_code + start_index), 
					(current_index - start_index));
			vector_push_back(tokens, token_);
		} else if(source_code[current_index] == '/') {
			current_index++;
			if (source_code[current_index] == '/') {
				current_index++;
				while (source_code[current_index] != '\n') {
					current_index++;
				}
			} else {
				token *token_ = create_token(SLASH, (source_code + start_index), 
						(current_index - start_index));
				vector_push_back(tokens, token_);
			}
		} else if(source_code[current_index] == '*') {
			current_index++;
			token *token_ = create_token(STAR, (source_code + start_index), 
					(current_index - start_index));
			vector_push_back(tokens, token_);
		} else if(source_code[current_index] == '=') {
			current_index++;
			if (source_code[current_index] == '='){
				current_index++;
				token *token_ = create_token(EQUAL_EQUAL, (source_code + start_index), 
						(current_index - start_index));
				vector_push_back(tokens, token_);
			} else {
				token *token_ = create_token(EQUAL, (source_code + start_index), 
						(current_index - start_index));
				vector_push_back(tokens, token_);
			}
		 
		} else if(source_code[current_index] == '!') {
			current_index++;
			if (source_code[current_index] == '='){
				current_index++;
				token *token_ = create_token(BANG_EQUAL, (source_code + start_index), 
						(current_index - start_index));
				vector_push_back(tokens, token_);
			} else {
				token *token_ = create_token(BANG, (source_code + start_index), 
						(current_index - start_index));
				vector_push_back(tokens, token_);
			}
		} else if(source_code[current_index] == '>') {
			current_index++;
			if (source_code[current_index] == '='){
				current_index++;
				token *token_ = create_token(GREATER_EQUAL, (source_code + start_index), 
						(current_index - start_index));
				vector_push_back(tokens, token_);
			} else {
				token *token_ = create_token(GREATER, (source_code + start_index), 
						(current_index - start_index));
				vector_push_back(tokens, token_);
			}
		} else if(source_code[current_index] == '<') {
			current_index++;
			if (source_code[current_index] == '='){
				current_index++;
				token *token_ = create_token(LESS_EQUAL, (source_code + start_index), 
						(current_index - start_index));
				vector_push_back(tokens, token_);
			} else {
				token *token_ = create_token(LESS, (source_code + start_index), 
						(current_index - start_index));
				vector_push_back(tokens, token_);
			}
		} else if(source_code[current_index] == '&') {
			current_index++;
			if (source_code[current_index] == '&') {
				current_index++;
				token *token_ = create_token(AND, (source_code + start_index), 
						(current_index - start_index));
				vector_push_back(tokens, token_);
			} else {
				perror("Invalid token '&'\n");
				current_index++;
			}
		} else if(source_code[current_index] == '|') {
			current_index++;
			if (source_code[current_index] == '|') {
				current_index++;
				token *token_ = create_token(OR, (source_code + start_index), 
						(current_index - start_index));
				vector_push_back(tokens, token_);
			} else {
				perror("Invalid token '|'\n");
				current_index++;
			}
		} else if (isalpha(source_code[current_index]) || source_code[current_index] == '_') {
			current_index++;
			while(isalpha(source_code[current_index]) 
					|| isdigit(source_code[current_index])
					|| source_code[current_index] == '_') {
				current_index++;
			}
			char *iden_buffer = create_token_string_copy(source_code, start_index, 
					current_index);
			token_type tok_type = get_keyword_token_from_string(iden_buffer);
			free(iden_buffer);
			token *token_ = create_token(tok_type, (source_code + start_index), 
					(current_index - start_index));
			vector_push_back(tokens, token_);
		} else if (source_code[current_index] == '"') {
			current_index++;
			while (source_code[current_index] != '"' && current_index < source_code_len) {
				current_index++;
			}
			if (source_code[current_index] == '"') {
				current_index++;
				token *token_ = create_token(STRING, (source_code + start_index + 1),
						(current_index - start_index - 2));
				vector_push_back(tokens, token_);
			} else {
				perror("Non-terminating string");
			}
		} else if (source_code[current_index] == ' ' 
				|| source_code[current_index] == '\n'
				|| source_code[current_index] == '\t'
				|| source_code[current_index] == '\r') {
			current_index++;
		}

		else {
			printf("Unsupported token '%c'\n", source_code[current_index]);
			current_index++;
		}
		start_index = current_index;
	}
	return tokens;
}




#endif
