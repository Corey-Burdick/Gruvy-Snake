cmake_minimum_required(VERSION 3.0...4.0)
project(snake C CXX)

include(FetchContent)

set(RAYLIB_VERSION 5.0)

FetchContent_Declare(
  raylib
  URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
)
FetchContent_MakeAvailable(raylib)

set(MY_SOURCES
    src/main.cpp
)

add_executable(${CMAKE_PROJECT_NAME} ${MY_SOURCES})

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE raylib)
