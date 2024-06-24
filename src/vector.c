#include "vector.h"

struct vector *vector_init() {
  struct vector *vector_ = calloc(1, sizeof(struct vector));
  if (!vector_) {
    perror("Vector initilization error");
    return NULL;
  }
  vector_->_internal_buffer =
      calloc(DEFAULT_INITIAL_SIZE, sizeof(GENERIC_TYPE_PTR));
  if (!vector_->_internal_buffer) {
    perror("Vector initilization error");
    free(vector_);
    return NULL;
  }
  vector_->size = 0;
  vector_->capacity = DEFAULT_INITIAL_SIZE;
  return vector_;
}

bool vector_push_back(struct vector *vector_, void *item) {
  assert(vector_ != NULL && item != NULL);
  if (vector_->capacity <= vector_->size) {
    if (!_vector_increase_capacity(vector_)) {
      return false;
    }
  }
  vector_->_internal_buffer[vector_->size++] = item;
  return true;
}

bool vector_replace_at(struct vector *vector_, size_t index, void *item) {
  assert(vector_ != NULL && index >= 0);
  if (vector_->size <= index) {
    return false;
  }
  vector_->_internal_buffer[index] = item;
  return true;
}

void *vector_remove_at(struct vector *vector_, size_t index) {
  assert(vector_ != NULL && index >= 0);
  if (vector_->size <= index) {
    return NULL;
  }
  void *returner = vector_at(vector_, index);
  size_t current_index = index;
  do {
    void *next_value_ptr = vector_->_internal_buffer[current_index + 1];
    vector_->_internal_buffer[current_index] = next_value_ptr;
    current_index++;
  } while (current_index < vector_->size);
  vector_->size--;
  return returner;
}

void *vector_at(struct vector *vector_, size_t index) {
  assert(vector_ != NULL && index >= 0);
  if (vector_->size <= index) {
    return NULL;
  }
  return vector_->_internal_buffer[index];
}

void vector_free(struct vector *vector_) {
  free(vector_->_internal_buffer);
  free(vector_);
}

bool _vector_increase_capacity(struct vector *vector_) {
  GENERIC_TYPE_PTR *temp_ptr =
      realloc(vector_->_internal_buffer,
              sizeof(GENERIC_TYPE_PTR) * vector_->capacity * 2);
  if (!temp_ptr) {
    return false;
  }
  vector_->_internal_buffer = temp_ptr;
  vector_->capacity = vector_->capacity * 2;
  return true;
}
