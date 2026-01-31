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
  QB_ERR_UNK
} QueryBuilderStatus;

QueryBuilderStatus qb_init(QueryBuilder* qb);
void qb_destroy(QueryBuilder* qb);
QueryBuilderStatus qb_and(QueryBuilder* qb);
QueryBuilderStatus qb_or(QueryBuilder* qb);

#endif
