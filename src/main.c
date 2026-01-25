#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "database.h"
#include "list.h"
#include "task.h"

static void print_tasks(const List* list) {
  for (size_t i = 0; i < list->size; ++i) {
    Task* task = &list->items[i];

    printf("- %d. [%c] %s\n", task->id, task->finished ? 'X' : ' ',
           task->title);
  }
}

int main(int argc, char** argv) {
  db_init();

  if (argc == 1) {
    List* tasks = db_list_tasks();
    print_tasks(tasks);
    destroy_list(tasks);
    db_close();
    return 0;
  }

  const char* command = argv[1];

  if (strcmp(command, "add") == 0) {
    if (argc < 3) {
      fprintf(stderr, "Missing task info.\n");
      exit(1);
    }

    Task* task = create_task(argv[2]);
    db_create_task(task);
    destroy_task(task);
    db_close();
    return 0;
  }

  if (strcmp(command, "check") == 0) {
    if (argc < 3) {
      fprintf(stderr, "Missing task ID.\n");
      exit(1);
    }

    int id = atoi(argv[2]);
    db_check_task(id);
    db_close();
    return 0;
  }

  if (strcmp(command, "del") == 0) {
    if (argc < 3) {
      fprintf(stderr, "Missing task ID.\n");
      exit(1);
    }

    int id = atoi(argv[2]);
    db_delete_task(id);
    db_close();
    return 0;
  }

  db_close();

  return 0;
}
