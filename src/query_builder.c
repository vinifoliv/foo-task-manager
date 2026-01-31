#include "query_builder.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SQL_INIT_SIZE 255
#define AND_SIZE 5
#define OR_SIZE 4

static int qb_grow_sql(QueryBuilder* qb) {
  if (!qb->sql) {
    fprintf(stderr, "The query builder SQL has not been initialized.\n");
    return QB_ERR_MEM;
  }

  char* buf = strdup(qb->sql);

  size_t new_size = qb->max_size * 2;
  qb->sql = realloc(qb->sql, new_size);
  strcpy(qb->sql, buf);

  free(buf);

  return QB_OK;
}

static char* qb_at_sql_end(QueryBuilder* qb) { return qb->sql + qb->size; }

static size_t qb_remaining_sql(QueryBuilder* qb) {
  return qb->max_size - qb->size;
}

int qb_init(QueryBuilder* qb) {
  char* sql = malloc(SQL_INIT_SIZE * sizeof(char));

  if (!sql) {
    fprintf(stderr, "Failed to malloc for SQL string.\n");
    return QB_ERR_MEM;
  }

  qb->sql = sql;
  qb->size = 0;
  qb->max_size = SQL_INIT_SIZE;

  return QB_OK;
}

void qb_destroy(QueryBuilder* qb) { free(qb->sql); }

int qb_clause(QueryBuilder* qb, const char* clause) {
  if (strstr(qb->sql, ";")) {
    fprintf(stderr, "Cannot add clause to SQL with ';'.\n");
    return QB_ERR_SYNTAX;
  }

  if (strlen(clause) > qb_remaining_sql(qb)) {
    int rc = qb_grow_sql(qb);
    if (rc != QB_OK) return rc;
  }

  qb->size += snprintf(qb_at_sql_end(qb), qb_remaining_sql(qb), "%s", clause);

  return QB_OK;
}

int qb_and(QueryBuilder* qb) {
  if (!strstr(qb->sql, "WHERE")) {
    fprintf(stderr, "Cannot append AND to statements missing WHERE.\n");
    return QB_ERR_SYNTAX;
  }

  size_t new_size = qb->size + AND_SIZE;

  if (new_size > qb_remaining_sql(qb)) {
    int rc = qb_grow_sql(qb);
    if (rc != QB_OK) return rc;
  }

  qb->size += snprintf(qb_at_sql_end(qb), qb_remaining_sql(qb), "%s", " AND ");

  return QB_OK;
}

int qb_or(QueryBuilder* qb) {
  if (!strstr(qb->sql, "WHERE")) {
    fprintf(stderr, "Cannot append OR to statements missing WHERE.\n");
    return QB_ERR_SYNTAX;
  }

  size_t new_size = qb->size + OR_SIZE;

  if (new_size > qb_remaining_sql(qb)) {
    int rc = qb_grow_sql(qb);
    if (rc != QB_OK) return rc;
  }

  qb->size += snprintf(qb_at_sql_end(qb), qb_remaining_sql(qb), "%s", " OR ");

  return QB_OK;
}
