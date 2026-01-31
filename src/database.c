#include "database.h"

#include <stdio.h>
#include <string.h>

#include "query_builder.h"
#include "sqlite3.h"
#include "task.h"

sqlite3* db = NULL;

static void finalize_stmt(sqlite3_stmt* stmt) {
  if (stmt) sqlite3_finalize(stmt);
}

QueryStatus db_init() {
  QueryStatus status = DB_ERR;

  if (db) {
    status = DB_OK;
    return status;
  }

  char* err;
  const char* sql =
      "CREATE TABLE IF NOT EXISTS task ("
      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
      "title VARCHAR(255) NOT NULL,"
      "description VARCHAR(255),"
      "finished BOOLEAN DEFAULT FALSE,"
      "created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP)";
  sqlite3_stmt* stmt = NULL;

  if (sqlite3_open("foo.db", &db) != SQLITE_OK) {
    fprintf(stderr, "Failed to open the database: %s.\n", sqlite3_errmsg(db));
    goto cleanup;
  }

  if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
    fprintf(stderr, "Failed to prepare SQLite statement: %s.\n",
            sqlite3_errmsg(db));
    goto cleanup;
  }

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    fprintf(stderr, "Failed to step through SQLite statement: %s.\n",
            sqlite3_errmsg(db));
    goto cleanup;
  }

  finalize_stmt(stmt);

  status = DB_OK;
  return status;

cleanup:
  finalize_stmt(stmt);
  if (db) db = NULL;
  return status;
}

QueryStatus db_close() {
  QueryStatus status = DB_ERR;

  if (!db) {
    status = DB_OK;
    return status;
  }

  if (sqlite3_close(db) != SQLITE_OK) {
    fprintf(stderr, "Failed to close database: %s.\n", sqlite3_errmsg(db));
    return status;
  }

  db = NULL;

  status = DB_OK;
  return status;
}

QueryStatus db_create_task(const Task* task) {
  QueryStatus status = DB_ERR;

  const char* sql =
      "INSERT INTO task(title, description, finished) VALUES(?, ?, ?)";
  sqlite3_stmt* stmt = NULL;

  if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
    fprintf(stderr, "Failed to prepare SQLite statement: %s.\n",
            sqlite3_errmsg(db));
    goto cleanup;
  }

  sqlite3_bind_text(stmt, 1, task->title, -1, SQLITE_TRANSIENT);
  sqlite3_bind_null(stmt, 2);
  sqlite3_bind_int(stmt, 3, task->finished);

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    fprintf(stderr, "Failed to step through SQLite statement: %s.\n",
            sqlite3_errmsg(db));
    goto cleanup;
  }

  finalize_stmt(stmt);

  status = DB_OK;
  return status;

cleanup:
  finalize_stmt(stmt);
  return status;
}

QueryStatus db_check_task(int id) {
  QueryStatus status = DB_ERR;

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

  status = DB_OK;
  return status;

cleanup:
  finalize_stmt(stmt);
  return status;
}

QueryStatus db_uncheck_task(int id) {
  QueryStatus status = DB_ERR;

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

  status = DB_OK;
  return status;

cleanup:
  finalize_stmt(stmt);
  return status;
}

QueryStatus db_list_task(int id, Task* task) {
  QueryStatus status = DB_ERR;

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
    status = DB_NOT_FOUND;
    return status;
  }

  if (rc == SQLITE_ROW) {
    task->id = sqlite3_column_int(stmt, 0);
    snprintf(task->title, TASK_TITLE_SIZE, "%s",
             (char*)sqlite3_column_text(stmt, 1));
    task->finished = sqlite3_column_int(stmt, 3);

    finalize_stmt(stmt);

    status = DB_OK;
    return status;
  }

  fprintf(stderr, "Failed to step through SQLite statement: %s.\n",
          sqlite3_errmsg(db));
  goto cleanup;

cleanup:
  finalize_stmt(stmt);
  return status;
}

QueryStatus db_list_tasks(List* tasks, Filter filter) {
  QueryStatus status = DB_ERR;
  QueryBuilder qb;

  if (qb_init(&qb) != QB_OK) goto cleanup;
  if (qb_clause(&qb, "SELECT * FROM task ") != QB_OK) goto cleanup;

  if (filter.done && filter.pending) {
    fprintf(stderr, "Cannot filter by done and pending at the same time.\n");
    goto cleanup;
  }

  if (filter.done) {
    if (qb_clause(&qb, "WHERE finished = TRUE") != QB_OK) goto cleanup;
  }

  if (filter.pending) {
    if (qb_clause(&qb, "WHERE finished = FALSE") != QB_OK) goto cleanup;
  }

  sqlite3_stmt* stmt = NULL;

  if (sqlite3_prepare_v2(db, qb.sql, -1, &stmt, NULL) != SQLITE_OK) {
    fprintf(stderr, "Failed to prepare SQLite statement: %s.\n",
            sqlite3_errmsg(db));
    goto cleanup;
  }

  int rc;

  while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
    int id = sqlite3_column_int(stmt, 0);

    char title[TASK_TITLE_SIZE] = "";
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
  qb_destroy(&qb);

  status = DB_OK;
  return status;

cleanup:
  finalize_stmt(stmt);
  qb_destroy(&qb);
  return status;
}

QueryStatus db_delete_task(int id) {
  QueryStatus status = DB_ERR;

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

  status = DB_OK;
  return status;

cleanup:
  finalize_stmt(stmt);
  return status;
}
