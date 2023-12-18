#ifndef tokens_h
#define tokens_h

#include <string.h>

typedef enum {
	LEFT_PAREN,
	RIGHT_PAREN,
	LEFT_BRACE,
	RIGHT_BRACE,
	COMMA,
	DOT,
	MINUS,
	PLUS,
	SEMICOLON,
	SLASH,
	STAR,
	BANG,
	EQUAL,
	EQUAL_EQUAL,
	BANG_EQUAL,
	GREATER,
	GREATER_EQUAL,
	LESS,
	LESS_EQUAL,
	AND,
	OR,
	IDENTIFIER,
	STRING,
	NUMBER,
	FUNCTION,
	IF,
	ELSE,
	TRUE,
	FALSE,
	NIL,
	VAR,
	RETURN,
	WHILE,
	FOR,
	EOF_TOKEN
} token_type;

token_type get_token_atom_from_string(const char *key) {
	if (strcmp(key, "(")) { return LEFT_PAREN; }
	if (strcmp(key, ")")) { return RIGHT_PAREN; }
	if (strcmp(key, "{")) { return LEFT_BRACE; }
	if (strcmp(key, "}")) { return RIGHT_BRACE; }
	if (strcmp(key, ",")) { return COMMA; }
	if (strcmp(key, ".")) { return DOT; }
	if (strcmp(key, "-")) { return MINUS; }
	if (strcmp(key, "+")) { return PLUS; }
	if (strcmp(key, ";")) { return SEMICOLON; }
	if (strcmp(key, "/")) { return SLASH; }
	if (strcmp(key, "*")) { return STAR; }
	if (strcmp(key, "!")) { return BANG; }
	if (strcmp(key, "=")) { return EQUAL; }
	if (strcmp(key, "==")) { return EQUAL_EQUAL; }
	if (strcmp(key, "!=")) { return BANG_EQUAL; }
	if (strcmp(key, ">")) { return GREATER; }
	if (strcmp(key, ">=")) { return GREATER_EQUAL; }
	if (strcmp(key, "<")) { return LESS; }
	if (strcmp(key, "<=")) { return LESS_EQUAL; }
	if (strcmp(key, "&&")) { return AND; }
	if (strcmp(key, "||")) { return OR; }
	if (strcmp(key, "FUNCTION")) { return FUNCTION; }
	if (strcmp(key, "if")) { return IF; }
	if (strcmp(key, "else")) { return ELSE; }
	if (strcmp(key, "true")) { return TRUE; }
	if (strcmp(key, "false")) { return FALSE; }
	if (strcmp(key, "nil")) { return NIL; }
	if (strcmp(key, "var")) { return VAR; }
	if (strcmp(key, "return")) { return RETURN; }
	if (strcmp(key, "while")) { return WHILE; }
	if (strcmp(key, "for")) { return FOR; }
	return -1;
}

const char *get_string_from_token_atom(token_type type) {
    switch (type) {
        case LEFT_PAREN:
            return "(";
        case RIGHT_PAREN:
            return ")";
        case LEFT_BRACE:
            return "{";
        case RIGHT_BRACE:
            return "}";
        case COMMA:
            return ",";
        case DOT:
            return ".";
        case MINUS:
            return "-";
        case PLUS:
            return "+";
        case SEMICOLON:
            return ";";
        case SLASH:
            return "/";
        case STAR:
            return "*";
        case BANG:
            return "!";
        case EQUAL:
            return "=";
        case EQUAL_EQUAL:
            return "==";
        case BANG_EQUAL:
            return "!=";
        case GREATER:
            return ">";
        case GREATER_EQUAL:
            return ">=";
        case LESS:
            return "<";
        case LESS_EQUAL:
            return "<=";
        case AND:
            return "&&";
        case OR:
            return "||";
        case FUNCTION:
            return "FUNCTION";
        case IF:
            return "if";
        case ELSE:
            return "else";
        case TRUE:
            return "true";
        case FALSE:
            return "false";
        case NIL:
            return "nil";
        case VAR:
            return "var";
        case RETURN:
            return "return";
        case WHILE:
            return "while";
        case FOR:
            return "for";
        default:
            return NULL;
    }
}


#endif
