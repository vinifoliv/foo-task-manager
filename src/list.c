#include "list.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "task.h"

static void grow_list(List* list) {
  size_t new_capacity = list->capacity * 2;

  list->items = realloc(list->items, list->capacity * sizeof(Task));

  if (!list->items) {
    fprintf(stderr, "Failed to grow list.\n");
    exit(1);
  }

  list->capacity = new_capacity;
}

List* create_list() {
  List* list = malloc(sizeof(List));

  if (!list) {
    fprintf(stderr, "Failed to create list.\n");
    exit(1);
  }

  list->items = malloc(LIST_INIT_CAP * sizeof(Task));
  list->size = 0;
  list->capacity = LIST_INIT_CAP;

  return list;
}

void add_to_list(List* list, int id, const char* title, bool finished) {
  if (list->size == list->capacity) grow_list(list);

  Task* task = &list->items[list->size++];

  task->id = id;
  strncpy(task->title, title, sizeof(task->title));
  task->finished = finished;
}

void destroy_list(List* list) {
  free(list->items);
  free(list);
}
