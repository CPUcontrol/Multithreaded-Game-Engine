cmake_minimum_required (VERSION 3.0)
project (gl3w)

add_library(gl3w STATIC src/gl3w.c)
target_include_directories(gl3w PUBLIC include)
