set(This ChessTests)

set(SOURCES tests.cpp)

add_executable(${This} ${SOURCES})
add_executable(SpeedTest speedtest.cpp)

target_link_libraries(${This} PRIVATE
  GTest::gtest_main
  ChessCore
)

target_link_libraries(SpeedTest PRIVATE
  GTest::gtest_main
  ChessCore
)

include(GoogleTest)
gtest_discover_tests(${This})
gtest_discover_tests(SpeedTest)
