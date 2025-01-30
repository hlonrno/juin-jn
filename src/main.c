#include "vec.h"
#include "lexer.h"
#include "result.h"
#include <limits.h>
#include <stdio.h>
#include <string.h>

void printArr(vec_t *arr, char *(*format)(void*, size_t)) {
  printf("[ ");
  for (size_t i = 0; i < arr->length - 2; i++)
    printf("%s, ", format(arr->data, i));

  printf("%s ]: ", format(arr->data, arr->length - 1));
  printf("len %zu, cap %zu;\n", arr->length, arr->capacity);
}

char *format_token(void *arr, size_t idx) {
  return "n";
}

int main(int argc, char **argv) {
  if (argc < 2)
    printf("Usage: jnc <file name1>");

  FILE* pFile = fopen(argv[1], "r");

  if (pFile == NULL) {
    printf("Couldn't read file %s", argv[1]);
    return 0;
  }

  result_t *result = tokenize(pFile);
  if (result->error[0]) {
    printf("ERROR: %s", result->error);
    return 1;
  }

  printArr((vec_t *)result->result, format_token);
}
