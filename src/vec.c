#include "vec.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Vector *vec_init(size_t initial_capacity, size_t element_size) {
  // ensure the size is 2^n
  size_t size = 1;
  for (; size < initial_capacity; size <<= 1);

  Vector *vec = (Vector *)malloc(sizeof(Vector));
  vec->data = calloc(size, element_size);
  vec->capacity = initial_capacity;
  vec->length = 0;
  vec->element_size = element_size;

  return vec;
}

void vec_destroy(Vector *vec) {
  free(vec->data);
  free(vec);
  vec = NULL;
}

void vec_check_new(Vector *vec, size_t new_elems) {
  if (vec->length + new_elems < vec->capacity)
    return;

  vec->capacity <<= 1;

  // L2 algorithm
  void *temp = calloc(vec->capacity, vec->element_size);
  memcpy(temp, vec->data, vec->length * vec->element_size);
  free(vec->data);

  vec->data = calloc(vec->capacity, vec->element_size);
  memcpy(vec->data, temp, vec->length * vec->element_size);
  free(temp);

  vec_check_new(vec, new_elems);
}

void vec_check_remove(Vector *vec, size_t removed_elems) {
  if (vec->length - removed_elems > vec->capacity >> 1)
    return;

  vec->capacity >>= 1;
  vec->data = realloc(vec->data, vec->capacity * vec->element_size);
}

void vec_ensure_capacity(Vector *vec, size_t capacity) {
  if (vec->capacity > capacity)
    return;

  vec_check_new(vec, capacity - vec->length);
}

void vec_add(Vector *vec, void *elem) {
  vec_check_new(vec, 1);
  memcpy(vec->data + vec->length * vec->element_size, elem, vec->element_size);
  vec->length++;
}

void vec_insert(Vector *vec, void *elem, size_t index) {
  vec_check_new(vec, 1);

  vec->length++;
  void *arr_elem = vec->data + index * vec->element_size;
  size_t len = (vec->length - index - 1) * vec->element_size;

  memcpy(arr_elem, arr_elem + vec->element_size, len);
  memcpy(arr_elem, elem, vec->element_size);
}

void *vec_pop(Vector *vec, size_t index) {
  vec->length--;
  void *elem = malloc(vec->element_size);
  void *arr_elem = vec->data + index * vec->element_size;
  size_t len = (vec->length - index) * vec->element_size;
  memcpy(elem, arr_elem, vec->element_size);
  memcpy(arr_elem, arr_elem + vec->element_size, len);
  vec_check_remove(vec, 1);

  return elem;
}

void vec_trim(Vector *vec) {
  vec->data = realloc(vec->data, vec->length * vec->element_size);
  vec->capacity = vec->length;
}
