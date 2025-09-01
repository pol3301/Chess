cmake_minimum_required(VERSION 3.31)
project(Chess CXX)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_COMPILER "clang++")

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE
      Debug
      CACHE STRING "Choose the build type" FORCE)
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

add_custom_target(
  copy_resources ALL COMMAND ${CMAKE_COMMAND} -E copy_directory
                             ${CMAKE_SOURCE_DIR}/res ${CMAKE_BINARY_DIR}/res)

set(SOURCES src/main.cpp src/renderer.cpp src/board.cpp src/game.cpp
            src/fen.cpp src/moves.cpp)

add_executable(Chess ${SOURCES})

add_dependencies(Chess copy_resources)

target_link_libraries(Chess PRIVATE SDL2::SDL2 SDL2_image::SDL2_image)
