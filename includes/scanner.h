#ifndef SCANNER_H
#define SCANNER_H

#include "tokens.h"
#include "utils.h"
#include "vector.h"
#include <ctype.h>
#include <stdlib.h>

/*
Returns vector of `struct token`
*/
struct vector *scan_tokens(const char *source_code);
char *parse_escape_sequences(const char *input);

#endif
