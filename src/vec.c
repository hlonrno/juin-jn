#include "vec.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

vec_t *vec_init(size_t initial_size, size_t element_size) {
  // ensure the size is 2^n
  size_t size = 1;
  for (; size < initial_size; size <<= 1);

  vec_t *arr = (vec_t *)malloc(sizeof(vec_t));
  arr->data = calloc(size, element_size);
  arr->capacity = initial_size;
  arr->element_size = element_size;

 return arr;
}

void vec_destroy(vec_t *arr) {
  free(arr->data);
  free(arr);
  arr = NULL;
}

void vec_check_new(vec_t *arr, size_t new_elems) {
  if (arr->length + new_elems < arr->capacity)
    return;

  arr->capacity <<= 1;

  // L2 algorithm
  void *temp = calloc(arr->capacity, arr->element_size);
  memcpy(temp, arr->data, arr->length * arr->element_size);
  free(arr->data);

  arr->data = calloc(arr->capacity, arr->element_size);
  memcpy(arr->data, temp, arr->length * arr->element_size);
  free(temp);

  vec_check_new(arr, new_elems);
}

void vec_check_remove(vec_t *arr, size_t removed_elems) {
  if (arr->length - removed_elems > arr->capacity >> 1)
    return;

  arr->capacity >>= 1;
  arr->data = realloc(arr->data, arr->capacity * arr->element_size);
}

void vec_ensure_capacity(vec_t *arr, size_t capacity) {
  if (arr->capacity > capacity)
    return;

  vec_check_new(arr, capacity - arr->length); 
}

void vec_add(vec_t *arr, void *elem) {
  vec_check_new(arr, 1);
  memcpy(arr->data + arr->length * arr->element_size, elem, arr->element_size);
  arr->length++;
}

void vec_insert(vec_t *arr, void *elem, size_t index) {
  vec_check_new(arr, 1);

  arr->length++;
  void *arr_elem = arr->data + index * arr->element_size;
  size_t len = (arr->length - index - 1) * arr->element_size;

  memcpy(arr_elem, arr_elem + arr->element_size, len);
  memcpy(arr_elem, elem, arr->element_size);
}

void *vec_pop(vec_t *arr, size_t index) {
  arr->length--;
  void *elem = malloc(arr->element_size);
  void *arr_elem = arr->data + index * arr->element_size;
  size_t len = (arr->length - index) * arr->element_size;
  memcpy(elem, arr_elem, arr->element_size);
  memcpy(arr_elem, arr_elem + arr->element_size, len);
  vec_check_remove(arr, 1);

  return elem;
}

void vec_trim(vec_t *arr) {
  arr->data = realloc(arr->data, arr->length * arr->element_size);
  arr->capacity = arr->length;
}
