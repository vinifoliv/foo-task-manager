#include "database.h"

#include <stdio.h>
#include <stdlib.h>

#include "list.h"
#include "sqlite3.h"
#include "task.h"

sqlite3* db = NULL;

void db_init() {
  if (db) return;

  char* err = NULL;
  int rc;

  rc = sqlite3_open("foo.db", &db);

  if (rc != SQLITE_OK) {
    fprintf(stderr, "Failed to open the database: %s\n", sqlite3_errmsg(db));
    exit(1);
  }

  const char* sql =
      "CREATE TABLE IF NOT EXISTS task ("
      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
      "title VARCHAR(255) NOT NULL,"
      "description VARCHAR(255),"
      "finished BOOLEAN DEFAULT FALSE,"
      "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP)";

  rc = sqlite3_exec(db, sql, NULL, NULL, &err);

  if (rc != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", err);
    sqlite3_free(err);
    exit(1);
  }
}

void db_close() {
  if (!db) return;

  int rc = sqlite3_close(db);

  if (rc != SQLITE_OK) {
    fprintf(stderr, "Failed to close database.\n");
    exit(1);
  }
}

void db_create_task(const Task* task) {
  const char* sql =
      "INSERT INTO task(title, description, finished) VALUES(?, ?, ?)";
  sqlite3_stmt* stmt;

  sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

  sqlite3_bind_text(stmt, 1, task->title, -1, SQLITE_STATIC);
  sqlite3_bind_null(stmt, 2);
  sqlite3_bind_int(stmt, 3, task->finished);

  sqlite3_step(stmt);
  sqlite3_finalize(stmt);
}

void db_check_task(int id) {
  sqlite3_stmt* stmt;
  const char* sql = "UPDATE task SET finished = TRUE WHERE id = ?";

  if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
    fprintf(stderr, "Failed to check task: %s\n", sqlite3_errmsg(db));
    exit(1);
  }

  sqlite3_bind_int(stmt, 1, id);
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);
}

void db_uncheck_task(int id) {
  const char* sql = "UPDATE task SET finished = FALSE WHERE id = ?";
  sqlite3_stmt* stmt;

  if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
    fprintf(stderr, "Failed to check task: %s\n", sqlite3_errmsg(db));
    exit(1);
  }

  sqlite3_bind_int(stmt, 1, id);
  sqlite3_step(stmt);
  sqlite3_finalize(stmt);
}

Task* db_list_task(int id) {
  sqlite3_stmt* stmt;
  const char* sql = "SELECT * FROM task WHERE id = ?";
  Task* task = NULL;

  if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
    fprintf(stderr, "Failed to prepare SQLite statement: %s\n",
            sqlite3_errmsg(db));
    exit(1);
  }

  if (sqlite3_bind_int(stmt, 1, id) != SQLITE_OK) {
    fprintf(stderr, "Failed to bind parameters to SQLite statement: %s\n",
            sqlite3_errmsg(db));
    exit(1);
  }

  int rc = sqlite3_step(stmt);

  if (rc == SQLITE_ROW) {
    int id = sqlite3_column_int(stmt, 0);
    const char* title = (char*)sqlite3_column_text(stmt, 1);
    bool finished = sqlite3_column_int(stmt, 3);
    sqlite3_finalize(stmt);

    task = new_task(id, title, finished);
    return task;
  }

  if (rc == SQLITE_DONE) {
    sqlite3_finalize(stmt);
    return task;
  }

  fprintf(stderr, "Failed to step through SQLite statement: %s\n",
          sqlite3_errmsg(db));
  exit(1);
}

List* db_list_tasks() {
  List* list = create_list();
  int rc;

  sqlite3_stmt* stmt;
  const char* sql = "SELECT * FROM task";

  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

  if (rc != SQLITE_OK) {
    fprintf(stderr, "Failed to list tasks: %s\n", sqlite3_errmsg(db));
    exit(1);
  }

  while (sqlite3_step(stmt) == SQLITE_ROW) {
    int id = sqlite3_column_int(stmt, 0);
    const char* title = (char*)sqlite3_column_text(stmt, 1);
    bool finished = sqlite3_column_int(stmt, 3);
    add_to_list(list, id, title, finished);
  }

  rc = sqlite3_finalize(stmt);

  if (rc != SQLITE_OK) {
    fprintf(stderr, "Failed to list tasks: %s\n", sqlite3_errmsg(db));
    exit(1);
  }

  return list;
}

void db_delete_task(int id) {
  sqlite3_stmt* stmt;
  const char* sql = "DELETE FROM task WHERE id = ?";
  int rc;

  rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);

  if (rc != SQLITE_OK) {
    fprintf(stderr, "Failed to delete task: %s\n", sqlite3_errmsg(db));
    exit(1);
  }

  sqlite3_bind_int(stmt, 1, id);

  sqlite3_step(stmt);

  sqlite3_finalize(stmt);

  if (rc != SQLITE_OK) {
    fprintf(stderr, "Failed to delete task: %s\n", sqlite3_errmsg(db));
    exit(1);
  }
}
