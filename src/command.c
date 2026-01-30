#include "command.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "database.h"
#include "task.h"

static void print_tasks(const List* list) {
  if (list->size == 0) printf("No tasks.\n");

  for (size_t i = 0; i < list->size; ++i) {
    Task* task = &list->items[i];

    printf("- %d. [%c] %s\n", task->id, task->finished ? 'x' : ' ',
           task->title);
  }
}

int run_command(int argc, const char** argv) {
  if (argc == 1) {
    list(argc, argv);
    return 0;
  }

  const char* command_name = argv[1];

  if (argc == 2 && strcmp(command_name, "--help") == 0) {
    printf("%s", general_help);
    return 0;
  }

  for (size_t i = 0; i < commands_count; ++i) {
    const Command* command = commands[i];

    if (strcmp(command->name, command_name) == 0 ||
        strcmp(command->alias, command_name) == 0) {
      command->function(argc, argv);

      return 0;
    }
  }

  fprintf(stderr, "Unknown command '%s'.\n", command_name);

  return ERR_INVALID_COMM;
}

int list(int argc, const char** argv) {
  if (argc == 3 && strcmp(argv[2], "--help") == 0) {
    printf("%s", list_command.help);
    return 0;
  }

  QueryResult result = db_list_tasks();

  if (result.status == QUERY_ERROR) {
    return ERR_DATABASE;
  }

  List* tasks = (List*)result.data;

  print_tasks(tasks);

  destroy_list(tasks);

  return 0;
}

int add(int argc, const char** argv) {
  if (argc < 3) {
    fprintf(stderr, "Missing task info.\n");
    return ERR_INVALID_ARGS;
  }

  if (strcmp(argv[2], "--help") == 0) {
    printf("%s", add_command.help);
    return 0;
  }

  const char* task_title = argv[2];

  Task* task = create_task(task_title);

  QueryResult result = db_create_task(task);

  destroy_task(task);

  if (result.status == QUERY_ERROR) {
    return ERR_DATABASE;
  }

  return 0;
}

int check(int argc, const char** argv) {
  if (argc < 3) {
    fprintf(stderr, "Missing task ID.\n");
    return ERR_INVALID_ARGS;
  }

  if (strcmp(argv[2], "--help") == 0) {
    printf("%s", check_command.help);
    return 0;
  }

  int id = atoi(argv[2]);

  QueryResult result = db_check_task(id);

  if (result.status == QUERY_ERROR) {
    return ERR_DATABASE;
  }

  return 0;
}

int uncheck(int argc, const char** argv) {
  if (argc < 3) {
    fprintf(stderr, "Missing task ID.\n");
    return ERR_INVALID_ARGS;
  }

  if (strcmp(argv[2], "--help") == 0) {
    printf("%s", uncheck_command.help);
    return 0;
  }

  int id = atoi(argv[2]);

  QueryResult result = db_list_task(id);

  if (result.status == QUERY_ERROR) {
    return ERR_DATABASE;
  }

  Task* task = (Task*)result.data;

  if (!task) {
    fprintf(stderr, "Not found.\n");
    return ERR_NOT_FOUND;
  }

  destroy_task(task);

  result = db_uncheck_task(id);

  if (result.status == QUERY_ERROR) {
    return ERR_DATABASE;
  }

  return 0;
}

int del(int argc, const char** argv) {
  if (argc < 3) {
    fprintf(stderr, "Missing task ID.\n");
    return ERR_INVALID_ARGS;
  }

  if (strcmp(argv[2], "--help") == 0) {
    printf("%s", del_command.help);
    return 0;
  }

  int id = atoi(argv[2]);

  QueryResult result = db_list_task(id);

  if (result.status == QUERY_ERROR) {
    return ERR_DATABASE;
  }

  Task* task = (Task*)result.data;

  if (!task) {
    fprintf(stderr, "Not found.\n");
    return ERR_NOT_FOUND;
  }

  destroy_task(task);

  result = db_delete_task(id);

  if (result.status == QUERY_ERROR) {
    return ERR_DATABASE;
  }

  return 0;
}
