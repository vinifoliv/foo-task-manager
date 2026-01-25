#include "task.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Task* create_task(const char* title) {
  Task* task = malloc(sizeof(Task));

  if (!task) {
    fprintf(stderr, "Failed to create task.\n");
    exit(1);
  }

  strcpy(task->title, title);
  task->finished = false;

  return task;
}

void destroy_task(Task* task) { free(task); }
