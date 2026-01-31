#include "database.h"

#include <stdio.h>
#include <string.h>

#include "sqlite3.h"
#include "task.h"

sqlite3* db = NULL;

static void finalize_stmt(sqlite3_stmt* stmt) {
  if (stmt) sqlite3_finalize(stmt);
}

QueryStatus db_init() {
  if (db) return DB_OK;

  char* err;
  const char* sql =
      "CREATE TABLE IF NOT EXISTS task ("
      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
      "title VARCHAR(255) NOT NULL,"
      "description VARCHAR(255),"
      "finished BOOLEAN DEFAULT FALSE,"
      "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP)";

  if (sqlite3_open("foo.db", &db) != SQLITE_OK) {
    fprintf(stderr, "Failed to open the database: %s.\n", sqlite3_errmsg(db));
    return DB_ERR;
  }

  if (sqlite3_exec(db, sql, NULL, NULL, &err) != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", err);
    sqlite3_free(err);
    return DB_ERR;
  }

  return DB_OK;
}

QueryStatus db_close() {
  if (!db) return DB_OK;

  if (sqlite3_close(db) != SQLITE_OK) {
    fprintf(stderr, "Failed to close database: %s.\n", sqlite3_errmsg(db));
    return DB_ERR;
  }

  return DB_OK;
}

QueryStatus db_create_task(const Task* task) {
  const char* sql =
      "INSERT INTO task(title, description, finished) VALUES(?, ?, ?)";
  sqlite3_stmt* stmt = NULL;

  if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
    fprintf(stderr, "Failed to prepare SQLite statement: %s.\n",
            sqlite3_errmsg(db));
    return DB_ERR;
  }

  sqlite3_bind_text(stmt, 1, task->title, -1, SQLITE_TRANSIENT);
  sqlite3_bind_null(stmt, 2);
  sqlite3_bind_int(stmt, 3, task->finished);

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    fprintf(stderr, "Failed to step through SQLite statement: %s.\n",
            sqlite3_errmsg(db));
    return DB_ERR;
  }

  finalize_stmt(stmt);

  return DB_OK;
}

QueryStatus db_check_task(int id) {
  const char* sql = "UPDATE task SET finished = TRUE WHERE id = ?";
  sqlite3_stmt* stmt = NULL;

  if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
    fprintf(stderr, "Failed to prepare SQLite statement: %s.\n",
            sqlite3_errmsg(db));
    goto cleanup;
  }

  sqlite3_bind_int(stmt, 1, id);

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    fprintf(stderr, "Failed to step through SQLite statement: %s.\n",
            sqlite3_errmsg(db));
    goto cleanup;
  }

  finalize_stmt(stmt);

  return DB_OK;

cleanup:
  if (stmt) finalize_stmt(stmt);
  return DB_ERR;
}

QueryStatus db_uncheck_task(int id) {
  const char* sql = "UPDATE task SET finished = FALSE WHERE id = ?";
  sqlite3_stmt* stmt = NULL;

  if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
    fprintf(stderr, "Failed to prepare SQLite statement: %s\n",
            sqlite3_errmsg(db));
    goto cleanup;
  }

  sqlite3_bind_int(stmt, 1, id);

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    fprintf(stderr, "Failed to step through SQLite statement: %s\n",
            sqlite3_errmsg(db));
    goto cleanup;
  }

  finalize_stmt(stmt);

  return DB_OK;

cleanup:
  if (stmt) finalize_stmt(stmt);
  return DB_ERR;
}

QueryStatus db_list_task(int id, Task* task) {
  const char* sql = "SELECT * FROM task WHERE id = ?";
  sqlite3_stmt* stmt = NULL;

  if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
    fprintf(stderr, "Failed to prepare SQLite statement: %s\n",
            sqlite3_errmsg(db));
    goto cleanup;
  }

  sqlite3_bind_int(stmt, 1, id);

  int rc = sqlite3_step(stmt);

  if (rc == SQLITE_DONE) {
    finalize_stmt(stmt);
    return DB_NOT_FOUND;
  }

  if (rc == SQLITE_ROW) {
    task->id = sqlite3_column_int(stmt, 0);
    strncpy(task->title, (char*)sqlite3_column_text(stmt, 1), TASK_TITLE_SIZE);
    task->finished = sqlite3_column_int(stmt, 3);

    finalize_stmt(stmt);

    return DB_OK;
  }

  fprintf(stderr, "Failed to step through SQLite statement: %s.\n",
          sqlite3_errmsg(db));
  goto cleanup;

cleanup:
  if (stmt) finalize_stmt(stmt);
  return DB_ERR;
}

QueryStatus db_list_tasks(List* tasks) {
  const char* sql = "SELECT * FROM task";
  sqlite3_stmt* stmt = NULL;

  if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
    fprintf(stderr, "Failed to prepare SQLite statement: %s.\n",
            sqlite3_errmsg(db));
    goto cleanup;
  }

  int rc;

  while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
    int id = sqlite3_column_int(stmt, 0);

    const char title[TASK_TITLE_SIZE];
    strncpy(title, (char*)sqlite3_column_text(stmt, 1), TASK_TITLE_SIZE);

    bool finished = sqlite3_column_int(stmt, 3);

    add_to_list(tasks, id, title, finished);
  }

  if (rc != SQLITE_DONE) {
    fprintf(stderr, "Failed to step through SQLite statement: %s.\n",
            sqlite3_errmsg(db));
    goto cleanup;
  }

  finalize_stmt(stmt);

  return DB_OK;

cleanup:
  if (stmt) finalize_stmt(stmt);
  return DB_ERR;
}

QueryStatus db_delete_task(int id) {
  const char* sql = "DELETE FROM task WHERE id = ?";
  sqlite3_stmt* stmt = NULL;

  if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
    fprintf(stderr, "Failed to prepare SQLite statement: %s\n",
            sqlite3_errmsg(db));
    goto cleanup;
  }

  sqlite3_bind_int(stmt, 1, id);

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    fprintf(stderr, "Failed to step through SQLite statement: %s.\n",
            sqlite3_errmsg(db));
    goto cleanup;
  }

  finalize_stmt(stmt);

  return DB_OK;

cleanup:
  if (stmt) finalize_stmt(stmt);
  return DB_ERR;
}
