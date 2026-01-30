#include "database.h"

#include <stdio.h>
#include <string.h>

#include "sqlite3.h"
#include "task.h"

sqlite3* db = NULL;

static void finalize_stmt(sqlite3_stmt* stmt) {
  if (stmt) sqlite3_finalize(stmt);
}

QueryResult db_init() {
  QueryResult result = {.status = QUERY_OK, .data = NULL};

  if (db) return result;

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
    result.status = QUERY_ERROR;
    return result;
  }

  if (sqlite3_exec(db, sql, NULL, NULL, &err) != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", err);
    sqlite3_free(err);
    result.status = QUERY_ERROR;
    return result;
  }

  return result;
}

QueryResult db_close() {
  QueryResult result = {.status = QUERY_OK, .data = NULL};

  if (!db) return result;

  if (sqlite3_close(db) != SQLITE_OK) {
    fprintf(stderr, "Failed to close database: %s.\n", sqlite3_errmsg(db));
    result.status = QUERY_ERROR;
    return result;
  }

  return result;
}

QueryResult db_create_task(const Task* task) {
  QueryResult result = {.status = QUERY_OK, .data = NULL};

  const char* sql =
      "INSERT INTO task(title, description, finished) VALUES(?, ?, ?)";
  sqlite3_stmt* stmt = NULL;

  if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
    fprintf(stderr, "Failed to prepare SQLite statement: %s.\n",
            sqlite3_errmsg(db));
    result.status = QUERY_ERROR;
    return result;
  }

  sqlite3_bind_text(stmt, 1, task->title, -1, SQLITE_TRANSIENT);
  sqlite3_bind_null(stmt, 2);
  sqlite3_bind_int(stmt, 3, task->finished);

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    fprintf(stderr, "Failed to step through SQLite statement: %s.\n",
            sqlite3_errmsg(db));
    result.status = QUERY_ERROR;
  }

  finalize_stmt(stmt);

  return result;
}

QueryResult db_check_task(int id) {
  QueryResult result = {.status = QUERY_OK, .data = NULL};

  const char* sql = "UPDATE task SET finished = TRUE WHERE id = ?";
  sqlite3_stmt* stmt = NULL;

  if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
    fprintf(stderr, "Failed to prepare SQLite statement: %s.\n",
            sqlite3_errmsg(db));
    result.status = QUERY_ERROR;
    finalize_stmt(stmt);
    return result;
  }

  sqlite3_bind_int(stmt, 1, id);

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    fprintf(stderr, "Failed to step through SQLite statement: %s.\n",
            sqlite3_errmsg(db));
    result.status = QUERY_ERROR;
  }

  finalize_stmt(stmt);

  return result;
}

QueryResult db_uncheck_task(int id) {
  QueryResult result = {.status = QUERY_OK, .data = NULL};

  const char* sql = "UPDATE task SET finished = FALSE WHERE id = ?";
  sqlite3_stmt* stmt = NULL;

  if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
    fprintf(stderr, "Failed to prepare SQLite statement: %s\n",
            sqlite3_errmsg(db));
    result.status = QUERY_ERROR;
    return result;
  }

  sqlite3_bind_int(stmt, 1, id);

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    fprintf(stderr, "Failed to step through SQLite statement: %s\n",
            sqlite3_errmsg(db));
    result.status = QUERY_ERROR;
  }

  finalize_stmt(stmt);

  return result;
}

QueryResult db_list_task(int id) {
  QueryResult result = {.status = QUERY_OK, .data = NULL};

  const char* sql = "SELECT * FROM task WHERE id = ?";
  sqlite3_stmt* stmt = NULL;

  if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
    fprintf(stderr, "Failed to prepare SQLite statement: %s\n",
            sqlite3_errmsg(db));
    result.status = QUERY_ERROR;
    return result;
  }

  sqlite3_bind_int(stmt, 1, id);

  int rc = sqlite3_step(stmt);

  if (rc == SQLITE_ROW) {
    int id = sqlite3_column_int(stmt, 0);
    const char* title = (char*)sqlite3_column_text(stmt, 1);
    bool finished = sqlite3_column_int(stmt, 3);

    Task* task = new_task(id, title, finished);

    finalize_stmt(stmt);

    if (result.status == QUERY_ERROR) {
      destroy_task(task);
      return result;
    }

    result.data = task;

    return result;
  }

  if (rc == SQLITE_DONE) {
    finalize_stmt(stmt);
    return result;
  }

  fprintf(stderr, "Failed to step through SQLite statement: %s.\n",
          sqlite3_errmsg(db));
  result.status = QUERY_ERROR;
  finalize_stmt(stmt);

  return result;
}

QueryResult db_list_tasks() {
  QueryResult result = {.status = QUERY_OK, .data = NULL};

  const char* sql = "SELECT * FROM task";
  sqlite3_stmt* stmt = NULL;

  if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
    fprintf(stderr, "Failed to prepare SQLite statement: %s.\n",
            sqlite3_errmsg(db));
    result.status = QUERY_ERROR;
    return result;
  }

  List* list = create_list();

  while ((sqlite3_step(stmt)) == SQLITE_ROW) {
    int id = sqlite3_column_int(stmt, 0);
    const char* title = strdup((char*)sqlite3_column_text(stmt, 1));
    bool finished = sqlite3_column_int(stmt, 3);
    add_to_list(list, id, title, finished);
  }

  finalize_stmt(stmt);

  if (result.status == QUERY_ERROR) {
    destroy_list(list);
    return result;
  }

  result.data = list;

  return result;
}

QueryResult db_delete_task(int id) {
  QueryResult result = {.status = QUERY_OK, .data = NULL};

  const char* sql = "DELETE FROM task WHERE id = ?";
  sqlite3_stmt* stmt = NULL;

  if (sqlite3_prepare_v2(db, sql, -1, &stmt, NULL) != SQLITE_OK) {
    fprintf(stderr, "Failed to prepare SQLite statement: %s\n",
            sqlite3_errmsg(db));
    result.status = QUERY_ERROR;
    return result;
  }

  sqlite3_bind_int(stmt, 1, id);

  if (sqlite3_step(stmt) != SQLITE_DONE) {
    fprintf(stderr, "Failed to step through SQLite statement: %s.\n",
            sqlite3_errmsg(db));
    result.status = QUERY_ERROR;
  }

  finalize_stmt(stmt);

  return result;
}
