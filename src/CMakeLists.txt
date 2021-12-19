cmake_minimum_required(VERSION 3.12)
add_executable(
  esquema
  esquema.cpp
)
target_link_libraries(esquema PRIVATE esquema-lib editline)

add_library(
  esquema-lib
  esquema/assert.h
  esquema/eval.h
  esquema/heap.h
  esquema/lex.h
  esquema/native.h
  esquema/parse.h
  esquema/print.h

  assert.cpp
  eval.cpp
  heap.cpp
  lex.cpp
  native.cpp
  parse.cpp
  print.cpp
)
target_include_directories(esquema-lib PUBLIC .)
