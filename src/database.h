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

QueryStatus db_init();
QueryStatus db_close();
QueryStatus db_create_task(const Task* task);
QueryStatus db_list_task(int id, Task* task);
QueryStatus db_list_tasks(List* tasks, Filter filter);
QueryStatus db_check_task(int id);
QueryStatus db_uncheck_task(int id);
QueryStatus db_delete_task(int id);

#endif
