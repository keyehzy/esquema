cmake_minimum_required(VERSION 3.12)
add_executable(
  esquema
  esquema.cpp
)
target_link_libraries(esquema PRIVATE esquema-lib editline)

add_library(
  esquema-lib
  esquema/lex.h
  esquema/print.h
  esquema/parse.h
  esquema/eval.h
  esquema/native.h

  lex.cpp
  print.cpp
  parse.cpp
  native.cpp
  eval.cpp
)
target_include_directories(esquema-lib PUBLIC .)
