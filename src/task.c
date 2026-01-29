#include "task.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Creates a Task object from all of its properties.
 * The title is copied into the object.
 * */
Task* new_task(int id, const char* title, bool finished) {
  Task* task = malloc(sizeof(Task));

  if (!task) {
    fprintf(stderr, "Failed to create task.\n");
    exit(1);
  }

  task->id = id;
  strcpy(task->title, title);
  task->finished = finished;

  return task;
}

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
