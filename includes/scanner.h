#ifndef SCANNER_H
#define SCANNER_H

#include "tokens.h"
#include "utils.h"
#include "vector.h"
#include <ctype.h>
#include <stdlib.h>

struct vector *scan_tokens(const char *source_code);

#endif
