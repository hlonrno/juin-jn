#include "lexer.c"
#include "token.c"

char *typeToString(TokenType type) {
  switch (type) {
    case KEYWORD:
      return "keyword";
    case IDENTIFIER:
      return "identifier";
    case OPERATOR:
      return "operator";
    case STRING_LITERAL:
      return "string_literal";
    case CHAR_LITERAL:
      return "char_literal";
    case INT_LITERAL:
      return "int_literal";
    case FLOAT_LITERAL:
      return "float_literal";
    case TYPE:
      return "type";
  }
  return "UNKNOWN";
}

int main() {
  Lexer lexer;
  lexerInit(&lexer, "../main.jn");

  Token buf[50];
  size_t buf_len = sizeof(buf) / sizeof(buf[0]);

  size_t length;
  while ((length = lexerTokenize(&lexer, buf, buf_len)) > 0) {
    for (size_t i = 0; i < length; ++i) {
      printf("|%s:%s|\n", typeToString(buf[i].type), buf[i].value.data);
      tokenDeinit(&buf[i]);
    }
  }

  if (lexer.error != NULL)
    printf("%s", lexer.error);

  lexerDeinit(&lexer);
}

