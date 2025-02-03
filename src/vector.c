#pragma once
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  size_t capacity;
  size_t size;
  size_t element_size;
  char *data;
} Vector;

void vectorResize(Vector *const restrict vector, size_t new_elements) {
  while (vector->capacity - vector->size < new_elements)
    vector->capacity <<= 2;

  char *temp = malloc(vector->capacity * vector->element_size);
  memcpy(temp, vector->data, vector->size);
  free (vector->data);

  vector->data = calloc(vector->capacity, vector->element_size);
  memcpy(vector->data, temp, vector->size);
  free(temp);
}

void vectorShrink(Vector *const restrict vector, size_t removed_elements) {
  while (vector->capacity >> 1 > vector->size - removed_elements) {
    vector->capacity >>= 1;
    vector->data = realloc(vector->data, vector->capacity * vector->element_size);
  }
}

void vectorInit(Vector *const restrict vector, size_t initial_capacity, size_t element_size) {
  vector->capacity = initial_capacity;
  vector->size = 0;
  vector->element_size = element_size;
  vector->data = calloc(initial_capacity, element_size);
}

void vectorDeinit(Vector *const restrict vector) {
  free(vector->data);
  vector->data = NULL;
  vector->size = 0;
  vector->capacity = 0;
  vector->element_size = 0;
}

void vectorEnsureCapacity(Vector *const restrict vector, size_t capacity) {
  vectorResize(vector, capacity);
}

void vectorAdd(Vector *const restrict vector, char *const restrict element) {
  vectorResize(vector, 1);
  memcpy(vector->data + vector->size * vector->element_size, element, vector->element_size);
  vector->size++;
}

void vectorPop(Vector *const restrict vector) {
  vector->size--;
  memset(vector->data + vector->size * vector->element_size, 0, vector->element_size);
  vectorShrink(vector, 1);
}

void vectorInsert(Vector *const restrict vector, char *const restrict element, size_t index) {
  vectorResize(vector, 1);
  char *array_element = vector->data + index * vector->element_size;
  size_t length = (vector->size - index) * vector->element_size;

  memcpy(array_element, array_element + vector->element_size, length);
  memcpy(array_element, element, vector->element_size);
  vector->size++;
}

void vectorRemove(Vector *const restrict vector, size_t index) {
  vector->size--;
  char *element = malloc(vector->element_size);
  char *array_element = vector->data + index * vector->element_size;
  size_t length = (vector->size - index) * vector->element_size;

  memcpy(element, array_element, vector->element_size);
  memcpy(array_element, array_element + vector->element_size, length);
  vectorShrink(vector, 1);
}

void vectorTrim(Vector *const restrict vector) {
  vector->capacity = vector->size;
  vector->data = realloc(vector->data, vector->size * vector->element_size);
}

