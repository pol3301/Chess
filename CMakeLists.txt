cmake_minimum_required(VERSION 3.31)
project(Chess CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Default build type = Release (but allow overriding)
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE
      Release
      CACHE STRING
            "Choose the build type (Debug, Release, RelWithDebInfo, MinSizeRel)"
            FORCE)
endif()

find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)

# Copy resources
add_custom_target(
  copy_resources ALL COMMAND ${CMAKE_COMMAND} -E copy_directory
                             ${CMAKE_SOURCE_DIR}/res ${CMAKE_BINARY_DIR}/res)

set(SOURCES src/main.cpp src/renderer.cpp src/board.cpp src/game.cpp
            src/fen.cpp src/moves.cpp)

add_executable(Chess ${SOURCES})

# Option to enable ASan
option(USE_ASAN "Enable AddressSanitizer" OFF)

if(USE_ASAN)
  target_compile_options(Chess PRIVATE -fsanitize=address
                                       -fno-omit-frame-pointer)
  target_link_options(Chess PRIVATE -fsanitize=address)
endif()

add_dependencies(Chess copy_resources)

target_link_libraries(Chess PRIVATE SDL2::SDL2 SDL2_image::SDL2_image)
