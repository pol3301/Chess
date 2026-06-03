cmake_minimum_required(VERSION 3.31)
project(Chess CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(This ChessCore)

include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.14.0
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

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

set(
  SOURCES
  src/renderer.cpp
  src/board.cpp
  src/game.cpp
  src/fen.cpp
  src/moves.cpp
  src/perft.cpp
)

# add_executable(${This} ${SOURCES})
add_library(${This} STATIC ${SOURCES})

add_executable(Chess src/main.cpp)
target_link_libraries(Chess ${This})

# Option to enable ASan
option(USE_ASAN "Enable AddressSanitizer" OFF)

if(USE_ASAN)
  target_compile_options(${This} PUBLIC -fsanitize=address
  -fno-omit-frame-pointer)
  target_link_options(${This} PUBLIC -fsanitize=address)
endif()

add_dependencies(${This} copy_resources)

target_link_libraries(${This} PUBLIC SDL2::SDL2 SDL2_image::SDL2_image)

add_subdirectory(tests)

target_include_directories(ChessCore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)
