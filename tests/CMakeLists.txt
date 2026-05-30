cmake_minimum_required(VERSION 3.8)

set(This Tests)

set(SOURCES test.cpp)

add_executable(${This} ${SOURCES})

target_link_libraries(${This} PUBLIC gtest_main ChessCore)

add_test(
  NAME ${This}
  PROGRAM ${This}
)
