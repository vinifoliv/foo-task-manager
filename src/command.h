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

static const Command commands[] = {
    {.name = "list", .alias = "", .function = list, .help = ""},
    {.name = "add", .alias = "a", .function = add, .help = ""},
    {.name = "check", .alias = "c", .function = check, .help = ""},
    {.name = "uncheck", .alias = "u", .function = uncheck, .help = ""},
    {.name = "del", .alias = "d", .function = del, .help = ""}};

static const size_t commands_count = sizeof(commands) / sizeof(Command);

#endif
