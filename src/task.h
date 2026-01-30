#ifndef TASK_H
#define TASK_H

#include <stdbool.h>
#include <stddef.h>

#define LIST_INIT_CAP 8

typedef struct {
  int id;
  char title[255];
  bool finished;
} Task;

typedef struct {
  Task* items;
  size_t size;
  size_t capacity;
} List;

Task* new_task(int id, const char* title, bool finished);
Task* create_task(const char* title);
void destroy_task(Task* task);

List* create_list();
void add_to_list(List* list, int id, const char* title, bool finished);
void destroy_list(List* list);

#endif
