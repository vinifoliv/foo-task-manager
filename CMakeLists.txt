cmake_minimum_required(VERSION 3.28.1)

project(foo LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(foo)

target_sources(foo PRIVATE
    src/main.c 
    src/task.c 
    src/command.c
    src/database.c 
    src/sqlite3.c 
)
