#include <stdbool.h>
#include <stddef.h>

#include "command.h"
#include "database.h"

int main(int argc, const char** argv) {
  db_init();
  int rc = run_command(argc, argv);
  db_close();
  return rc;
}
