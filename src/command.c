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

    printf("- % 3d. [%c] %s\n", task->id, task->finished ? 'x' : ' ',
           task->title);
  }
}

int run_command(int argc, const char** argv) {
  if (argc == 1) {
    list(argc, argv);
    return COMM_OK;
  }

  const char* command_name = argv[1];

  if (argc == 2 && strcmp(command_name, "--help") == 0) {
    printf("%s", general_help);
    return COMM_OK;
  }

  for (size_t i = 0; i < commands_count; ++i) {
    const Command* command = commands[i];

    if (strcmp(command->name, command_name) == 0 ||
        strcmp(command->alias, command_name) == 0) {
      command->function(argc, argv);

      return COMM_OK;
    }
  }

  fprintf(stderr, "Unknown command '%s'.\n", command_name);

  return COMM_ERR_INVALID_COMM;
}

int list(int argc, const char** argv) {
  if (argc == 3 && strcmp(argv[2], "--help") == 0) {
    printf("%s", list_command.help);
    return COMM_OK;
  }

  List* tasks = create_list();

  if (db_list_tasks(tasks) != DB_OK) {
    return COMM_ERR_DATABASE;
  }

  print_tasks(tasks);

  destroy_list(tasks);

  return COMM_OK;
}

int add(int argc, const char** argv) {
  if (argc < 3) {
    fprintf(stderr, "Missing task info.\n");
    return COMM_ERR_INVALID_ARGS;
  }

  if (strcmp(argv[2], "--help") == 0) {
    printf("%s", add_command.help);
    return COMM_OK;
  }

  const char* task_title = argv[2];

  Task* task = create_task(task_title);

  if (db_create_task(task) != DB_OK) {
    destroy_task(task);
    return COMM_ERR_DATABASE;
  }

  destroy_task(task);

  return COMM_OK;
}

int check(int argc, const char** argv) {
  if (argc < 3) {
    fprintf(stderr, "Missing task ID.\n");
    return COMM_ERR_INVALID_ARGS;
  }

  if (strcmp(argv[2], "--help") == 0) {
    printf("%s", check_command.help);
    return COMM_OK;
  }

  int id = atoi(argv[2]);
  Task task;

  int rc = db_list_task(id, &task);

  if (rc == DB_ERR) {
    return COMM_ERR_DATABASE;
  }

  if (rc == DB_NOT_FOUND) {
    fprintf(stderr, "Not found.\n");
    return COMM_ERR_NOT_FOUND;
  }

  if (db_check_task(id) != DB_OK) {
    return COMM_ERR_DATABASE;
  }

  return COMM_OK;
}

int uncheck(int argc, const char** argv) {
  if (argc < 3) {
    fprintf(stderr, "Missing task ID.\n");
    return COMM_ERR_INVALID_ARGS;
  }

  if (strcmp(argv[2], "--help") == 0) {
    printf("%s", uncheck_command.help);
    return COMM_OK;
  }

  int id = atoi(argv[2]);
  Task task;

  int rc = db_list_task(id, &task);

  if (rc == DB_ERR) {
    return COMM_ERR_DATABASE;
  }

  if (rc == DB_NOT_FOUND) {
    fprintf(stderr, "Not found.\n");
    return COMM_ERR_NOT_FOUND;
  }

  if (db_uncheck_task(id)) {
    return COMM_ERR_DATABASE;
  }

  return COMM_OK;
}

int del(int argc, const char** argv) {
  if (argc < 3) {
    fprintf(stderr, "Missing task ID.\n");
    return COMM_ERR_INVALID_ARGS;
  }

  if (strcmp(argv[2], "--help") == 0) {
    printf("%s", del_command.help);
    return COMM_OK;
  }

  int id = atoi(argv[2]);
  Task task;

  int rc = db_list_task(id, &task);

  if (rc == DB_ERR) {
    return COMM_ERR_DATABASE;
  }

  if (rc == DB_NOT_FOUND) {
    fprintf(stderr, "Not found.\n");
    return COMM_ERR_NOT_FOUND;
  }

  if (db_delete_task(id) == DB_OK) {
    return COMM_ERR_DATABASE;
  }

  return COMM_OK;
}
