#include "query_builder.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SQL_INIT_SIZE 256
#define AND_SIZE 5
#define OR_SIZE 4

static QueryBuilderStatus qb_grow_sql(QueryBuilder* qb) {
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

QueryBuilderStatus qb_init(QueryBuilder* qb) {
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

QueryBuilderStatus qb_and(QueryBuilder* qb) {
  if (!strstr(qb->sql, "WHERE")) {
    return QB_ERR_SYNTAX;
  }

  if (qb->size + AND_SIZE > qb->size) {
    int rc = qb_grow_sql(qb);
    if (rc != QB_OK) return rc;
  }

  qb->size +=
      snprintf(qb->sql + qb->size, qb->max_size - qb->size, "%s", " AND ");

  return QB_OK;
}

QueryBuilderStatus qb_or(QueryBuilder* qb) {
  if (!strstr(qb->sql, "WHERE")) {
    return QB_ERR_SYNTAX;
  }

  if (qb->size + OR_SIZE > qb->size) {
    int rc = qb_grow_sql(qb);
    if (rc != QB_OK) return rc;
  }

  qb->size +=
      snprintf(qb->sql + qb->size, qb->max_size - qb->size, "%s", " OR ");

  return QB_OK;
}
