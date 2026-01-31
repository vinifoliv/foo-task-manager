#ifndef QUERY_BUILDER_H
#define QUERY_BUILDER_H

#include <stddef.h>

typedef struct {
  char* sql;
  size_t size;
  size_t max_size;
} QueryBuilder;

typedef enum {
  QB_OK,
  QB_ERR_MEM,
  QB_ERR_SYNTAX,
  QB_ERR_SQLLEN,
  QB_ERR_UNK
} QueryBuilderStatus;

int qb_init(QueryBuilder* qb);
void qb_destroy(QueryBuilder* qb);
int qb_clause(QueryBuilder* qb, const char* clause);
int qb_and(QueryBuilder* qb);
int qb_or(QueryBuilder* qb);

#endif
