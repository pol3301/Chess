set(This ChessTests)

set(SOURCES tests.cpp)

add_executable(${This} ${SOURCES})

target_link_libraries(${This} PRIVATE
  GTest::gtest_main
  ChessCore
)

include(GoogleTest)
gtest_discover_tests(${This})
