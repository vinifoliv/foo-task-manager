#ifndef COMMAND_H
#define COMMAND_H

#include <stddef.h>

typedef enum {
  ERR_OK,
  ERR_INVALID_COMM,
  ERR_INVALID_ARGS,
  ERR_NOT_FOUND,
  ERR_DATABASE,
} Error;

typedef int (*CommandFunction)(int argc, const char** argv);

typedef struct {
  const char* name;
  const char* alias;
  CommandFunction function;
  const char* help;
} Command;

int run_command(int argc, const char** argv);

int list(int argc, const char** argv);
int add(int argc, const char** argv);
int check(int argc, const char** argv);
int uncheck(int argc, const char** argv);
int del(int argc, const char** argv);

static const char* general_help =
    "foo - simple and fast task manager\n"
    "\n"
    "Usage:\n"
    "  foo <command> [options]\n"
    "  foo            List all tasks (default)\n"
    "\n"
    "Commands:\n"
    "  list        List all tasks\n"
    "  add         Add a new task\n"
    "  check       Mark a task as completed\n"
    "  uncheck     Mark a task as pending\n"
    "  del         Delete a task\n"
    "\n"
    "Options:\n"
    "  --help      Show this help message\n";

static const Command list_command = {
    .name = "list",
    .alias = "ps",
    .function = list,
    .help =
        "List all tasks.\n"
        "Usage: foo list\n"
        "Shows pending and completed tasks.\n"};

static const Command add_command = {.name = "add",
                                    .alias = "a",
                                    .function = add,
                                    .help =
                                        "Add a new task.\n"
                                        "Usage: foo add <title>\n"
                                        "Example: foo add \"Study SQLite\"\n"};

static const Command check_command = {.name = "check",
                                      .alias = "c",
                                      .function = check,
                                      .help =
                                          "Mark a task as completed.\n"
                                          "Usage: foo check <id>\n"
                                          "Example: foo check 3\n"};

static const Command uncheck_command = {
    .name = "uncheck",
    .alias = "u",
    .function = uncheck,
    .help =
        "Mark a completed task as pending again.\n"
        "Usage: foo uncheck <id>\n"
        "Example: foo uncheck 3\n"};

static const Command del_command = {.name = "del",
                                    .alias = "d",
                                    .function = del,
                                    .help =
                                        "Delete a task.\n"
                                        "Usage: foo del <id>\n"
                                        "Example: foo del 3\n"};

static const Command* commands[] = {&list_command, &add_command, &check_command,
                                    &uncheck_command, &del_command};

static const size_t commands_count = sizeof(commands) / sizeof(Command*);

#endif
