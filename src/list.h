#ifndef LIST_H
#define LIST_H

#include <stdbool.h>
#include <stddef.h>

typedef struct {
  void* items;
  size_t size;
  size_t capacity;
  size_t item_size;
} List;

bool is_full(const List* list);

bool is_empty(const List* list);

void init_list(List* list, size_t item_size);

void destroy_list(List* list);

void append(List* list, void* item);

void* get_at(const List* list, int index);

#endif
