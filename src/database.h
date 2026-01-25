#ifndef DATABASE_H
#define DATABASE_H

#include "list.h"
#include "task.h"

void db_init();
void db_close();
void db_create_task(const Task* Task);
List* db_list_tasks();
void db_check_task(int id);
void db_delete_task(int id);

#endif
