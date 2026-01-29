#ifndef TASK_H
#define TASK_H

#include <stdbool.h>

typedef struct {
  int id;
  char title[255];
  bool finished;
} Task;

Task* new_task(int id, const char* title, bool finished);
Task* create_task(const char* title);
void destroy_task(Task* task);

#endif
