#include "lexer.h"
#include "token.h"
#include "vec.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
  if (argc < 2) {
    printf("Usage: jnc <file name1>\n");
    return 0;
  }
  
  FILE *fptr = fopen(argv[1], "r");

  if (fptr == NULL) {
    printf("Couldn't read file %s", argv[1]);
    return 0;
  }

  printf("[1/4] Tokenization.\n");
  Lexer *lexer = tokenize(fptr);
  if (lexer->error) {
    printf("Error: %s\n", lexer->error_message);
    return 0;
  }

  for (size_t i = 0; i < lexer->tokens->length; i++) {
    Token *token = (Token *)(lexer->tokens->data + i);

    switch (token->type) {
    case KEYWORD:
      printf("<keyword:%s>", (char *)token->value.arr->data);
      break;
    case IDENTIFIER:
      printf("<identifier:%s>", (char *)token->value.arr->data);
      break;
    case OPERATOR:
      printf("<operator:%s>", token->value.str);
      break;
    case STR_LITERAL:
      printf("<str_literal:%s>", (char *)token->value.arr->data);
      break;
    case CHR_LITERAL:
      printf("<chr_literal:%s>", token->value.str);
      break;
    case INT_LITERAL:
      printf("<int_literal:%lld>", token->value.num);
      break;
    case FLT_LITERAL:
      printf("<flt_literal:%f>", token->value.flt);
    }
  }

  vec_destroy(lexer->tokens);
  free(lexer);
  fclose(fptr);
  return 0;
}
