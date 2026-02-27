#include "ui.h"

#include <stdio.h>

#include "list.h"
#include "task.h"

void display(const List* list) {
  if (list->size == 0) printf("No tasks found.\n");

  for (size_t i = 0; i < list->size; ++i) {
    Task* task = (Task*)get_at(list, i);

    const char* finished = task->finished ? "✓" : " ";

    printf("% 2d. [%s] %s\n", task->id, finished, task->title);
  }
}
