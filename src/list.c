#include "list.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LIST_INITIAL_CAPACITY 8

static void grow_list(List* list) {
  if (!list) {
    fprintf(stderr, "List has already been initialized.\n");
    exit(1);
  }

  size_t new_capacity = list->capacity * 2;

  list->items = realloc(list->items, new_capacity * list->item_size);

  if (!list->items) {
    fprintf(stderr, "Failed to realloc list items.\n");
    exit(1);
  }

  list->capacity = new_capacity;
}

bool is_full(const List* list) { return list->size == list->capacity; }

bool is_empty(const List* list) { return list->size == 0; }

void init_list(List* list, size_t item_size) {
  list->size = 0;
  list->capacity = LIST_INITIAL_CAPACITY;
  list->item_size = item_size;
  list->items = malloc(LIST_INITIAL_CAPACITY * item_size);

  if (!list->items) {
    fprintf(stderr, "Failed to initialize list.\n");
    exit(1);
  }
}

void destroy_list(List* list) {
  if (!list) return;

  free(list->items);
  list->size = 0;
  list->capacity = 0;
  list->item_size = 0;
}

void append(List* list, void* item) {
  if (is_full(list)) grow_list(list);

  void* last_position = (char*)list->items + list->size * list->item_size;

  memcpy(last_position, item, list->item_size);

  list->size++;
}

void* get_at(const List* list, int index) {
  return (char*)list->items + index * list->item_size;
}
