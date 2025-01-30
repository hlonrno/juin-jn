#include "lexer.h"
#include "tokens.h"
#include <stdlib.h>
#include <string.h>

#define IS_NUMBER(n) (n > 47 && n < 58)

char *clean(char *buf) {
  for (size_t i = 0; buf[i] != '\0'; i++)
    buf[i] = buf[i] * (buf[i] != '\n') + '^' * (buf[i] == '\n');

  return buf;
}

result_t *tokenize(FILE *pFile) {
  vec_t *tokens = vec_init(1024, sizeof(token_t));
  result_t *result = malloc(sizeof(result_t));
  result->result = tokens;

  char buf[1024];
  char last_task = 0;
  size_t idx = 0;
  char chr;

  if (!fgets(buf, sizeof(buf), pFile)) {
    result->error = "Couldn't read file.";
    return result;
  }

  for (;;) {
  next_chr:
    idx++;
    if (idx > sizeof(buf)) {
      idx %= sizeof(buf);
      if (!fgets(buf, sizeof(buf), pFile))
        break;
    }
    chr = buf[idx];
    switch (last_task) {
    case 1:
      goto number;
    case 2:
      goto string;
    case 3:
      goto special_char;
    case 4:
      goto character;
    case 5:
      goto identifier;
    }

    if (IS_NUMBER(chr)) {
      last_task = 1;
      unsigned long long num = 0;
      char dot_count = 0;
      unsigned long long radix = 1;

    number:
      while (IS_NUMBER(chr)) {
        if (chr == '.') {
          dot_count++;
          if (dot_count > 1)
            break;
        }
        num += (chr - '0') * radix;
        radix = (radix << 1) + (radix << 3);
        goto next_chr;
      }

      token_value_t *val = malloc(sizeof(token_value_t));
      val->number = num;
      vec_add(tokens, token_init(NUM_LITERAL, *val));
      last_task = 0;
    } else if (chr == '"') {
      last_task = 2;
      vec_t *str = vec_init(32, sizeof(chr));
      goto next_chr;

    string:
      while (chr != '"') {
        vec_add(str, &chr);
        goto next_chr;
      }

      vec_trim(str);
      token_value_t *val = malloc(sizeof(token_value_t));
      val->arr = str;
      vec_add(tokens, token_init(STR_LITERAL, *val));
      last_task = 0;
      goto next_chr;
    } else if (strchr(SPECIAL_CHARS, chr)) {
      last_task = 3;
      token_value_t *val = malloc(sizeof(token_value_t));

    special_char:
      val->chr = chr;
      vec_add(tokens, token_init(SPECIAL_CHAR, *val));
      last_task = 0;
      goto next_chr;
    } else if (chr == '\'') {
      last_task = 4;
      token_value_t *val = malloc(sizeof(token_value_t));
      goto next_chr;

    character:
      val->chr = chr;
      vec_add(tokens, token_init(CHR_LITERAL, *val));
      last_task = 0;
      idx++;
      goto next_chr;
    } else if (strchr(LETTERS, chr)) {
      last_task = 5;
      vec_t *ident = vec_init(16, sizeof(char));

    identifier:
      while (strchr(IDENT, chr)) {
        vec_add(ident, &chr);
        goto next_chr;
      }

      token_value_t *val = malloc(sizeof(token_value_t));
      val->arr = ident;
      vec_add(tokens, token_init(IDENTIFIER, *val));
      last_task = 0;
    } else if (strchr(" \n\t\r\0", chr)) {
      goto next_chr;
    } else {
      result->error = "Couldn't find token.";
      printf("chr: %c", buf[idx]);
      printf("\n%zu: %s\n", idx, buf);
      return result;
    }
  }

  return result;
}
