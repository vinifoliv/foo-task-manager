#ifndef LIST_H
#define LIST_H

#include <stddef.h>

#include "task.h"

#define LIST_INIT_CAP 8

typedef struct {
  Task* items;
  size_t size;
  size_t capacity;
} List;

List* create_list();
void add_to_list(List* list, int id, const char* title, bool finished);
void destroy_list(List* list);

#endif
