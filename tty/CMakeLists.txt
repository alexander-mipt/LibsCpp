cmake_minimum_required(VERSION 3.0)
project(Utty VERSION 1.0 LANGUAGES C CXX)

add_library(tty SHARED tty.c)
#target_include_directories(tty PRIVATE include)
target_compile_options(tty PRIVATE -Wall -Wextra -Werror)