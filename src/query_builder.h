#ifndef QUERY_BUILDER_H
#define QUERY_BUILDER_H

#include <stddef.h>

typedef struct {
  char* buffer;
  size_t size;
  size_t capacity;
} QueryBuilder;

typedef enum {
  QB_OK,
  QB_ERROR,
} QueryBuilderStatus;

int qb_init(QueryBuilder* qb);
void qb_destroy(QueryBuilder* qb);
int qb_clause(QueryBuilder* qb, const char* clause);
int qb_from(QueryBuilder* qb, const char* table);
int qb_where(QueryBuilder* qb, const char* condition);
int qb_and(QueryBuilder* qb);
int qb_or(QueryBuilder* qb);

#endif
