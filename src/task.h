#ifndef TASK_H
#define TASK_H

#include <stdbool.h>
#include <stddef.h>

#define TASK_TITLE_SIZE 255

typedef struct {
  int id;
  char title[TASK_TITLE_SIZE];
  bool finished;
} Task;

Task* new_task(int id, const char* title, bool finished);
Task* create_task(const char* title);
void destroy_task(Task* task);

#endif
