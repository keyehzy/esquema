cmake_minimum_required(VERSION 3.18)

add_executable(
  esquema-test
  test-lex-parse.cpp
  test-level2.cpp
  test-heap.cpp
)
target_include_directories(esquema-test PRIVATE .)
target_link_libraries(
  esquema-test
  PRIVATE
  gmock
  gmock_main
  gtest
  esquema-lib
)

add_test(
  NAME esquema-test
  COMMAND esquema-test
)
