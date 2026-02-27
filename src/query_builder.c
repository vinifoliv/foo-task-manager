#include "query_builder.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_INITIAL_CAPACITY 255
#define SPACEMENT_SIZE 2
#define SELECT "SELECT"
#define FROM "FROM"
#define WHERE "WHERE"
#define AND "AND"
#define OR "OR"

static int qb_grow_buffer(QueryBuilder* qb) {
  if (!qb->buffer) {
    fprintf(stderr, "Query builder buffer has not been initialized.\n");
    return QB_ERROR;
  }

  char* buf = strdup(qb->buffer);

  size_t new_capacity = qb->capacity * 2;
  qb->buffer = realloc(qb->buffer, new_capacity);

  if (!qb->buffer) {
    fprintf(stderr, "Failed to grow query builder buffer.\n");
    return QB_ERROR;
  }

  qb->capacity = new_capacity;
  strncpy(qb->buffer, buf, qb->size);

  free(buf);

  return QB_OK;
}

static char* qb_buffer_end(QueryBuilder* qb) { return qb->buffer + qb->size; }

static size_t qb_available_space(QueryBuilder* qb) {
  return qb->capacity - qb->size;
}

static int qb_add_to_buffer(QueryBuilder* qb, const char* value) {
  if (!qb->buffer) {
    fprintf(stderr, "Query builder buffer has not been initialized.\n");
    return QB_ERROR;
  }

  size_t new_size = qb->size + SPACEMENT_SIZE + strlen(value);

  if (new_size > qb_available_space(qb)) {
    int rc = qb_grow_buffer(qb);
    if (rc != QB_OK) return rc;
  }

  qb->size +=
      snprintf(qb_buffer_end(qb), qb_available_space(qb), " %s ", value);

  return QB_OK;
}

int qb_init(QueryBuilder* qb) {
  char* buffer = malloc(BUFFER_INITIAL_CAPACITY * sizeof(char));

  if (!buffer) {
    fprintf(stderr, "Failed to malloc query builder buffer.\n");
    return QB_ERROR;
  }

  qb->buffer = buffer;
  qb->size = 0;
  qb->capacity = BUFFER_INITIAL_CAPACITY;

  return QB_OK;
}

void qb_destroy(QueryBuilder* qb) {
  if (!qb->buffer) {
    free(qb->buffer);
  }
}

int qb_clause(QueryBuilder* qb, const char* clause) {
  return qb_add_to_buffer(qb, clause);
}

int qb_select(QueryBuilder* qb, const char* fields) {
  int rc = qb_add_to_buffer(qb, SELECT) != QB_OK;

  if (rc != QB_OK) return rc;

  return qb_add_to_buffer(qb, fields);
}

int qb_from(QueryBuilder* qb, const char* table) {
  int rc = qb_add_to_buffer(qb, FROM) != QB_OK;

  if (rc != QB_OK) return rc;

  return qb_add_to_buffer(qb, table);
}

int qb_where(QueryBuilder* qb, const char* condition) {
  int rc = qb_add_to_buffer(qb, WHERE) != QB_OK;

  if (rc != QB_OK) return rc;

  return qb_add_to_buffer(qb, condition);
}

int qb_and(QueryBuilder* qb) { return qb_add_to_buffer(qb, AND); }

int qb_or(QueryBuilder* qb) { return qb_add_to_buffer(qb, OR); }
