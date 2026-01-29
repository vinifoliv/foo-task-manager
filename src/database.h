#ifndef DATABASE_H
#define DATABASE_H

#include "task.h"

typedef enum {
  QUERY_OK,
  QUERY_ERROR,
} QueryStatus;

typedef struct {
  QueryStatus status;
  void* data;
} QueryResult;

QueryResult db_init();
QueryResult db_close();
QueryResult db_create_task(const Task* Task);
QueryResult db_list_task(int id);
QueryResult db_list_tasks();
QueryResult db_check_task(int id);
QueryResult db_uncheck_task(int id);
QueryResult db_delete_task(int id);

#endif
