#include "ui.h"

#include <stdio.h>

void display(const List* list) {
  if (list->size == 0) printf("No tasks found.\n");

  for (size_t i = 0; i < list->size; ++i) {
    Task* task = &list->items[i];

    printf("% 2d. [%s] %s\n", task->id, task->finished ? "✓" : " ",
           task->title);
  }
}
