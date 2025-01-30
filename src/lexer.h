#pragma once

#include "vec.h"
#include "result.h"
#include <stdio.h>

#define SPECIAL_CHARS "#{}()[]:.,;=*/+-"
#define LETTERS "_qwertyuiopadfghjklzxcvbnmQWERTYUIOPADFGHJKLZXCVBNM"
#define NUMBERS "1234567890"
#define IDENT "_0123456789qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM"

result_t *tokenize(FILE *pFile);

