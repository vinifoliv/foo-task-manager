#ifndef DATABASE_H
#define DATABASE_H

#include "task.h"

typedef enum {
  DB_OK,
  DB_NOT_FOUND,
  DB_ERR,
} QueryStatus;

typedef struct {
  bool done;
  bool pending;
} Filter;

int db_init();
int db_close();
int db_create_task(const Task* task);
int db_list_task(int id, Task* task);
int db_list_tasks(List* tasks, Filter filter);
int db_check_task(int id);
int db_uncheck_task(int id);
int db_delete_task(int id);

#endif
