cmake_minimum_required(VERSION 4.0)
project(snake C CXX)

include_directories(/usr/local/include)

link_directories(/usr/local/lib)

set(MY_SOURCES
    src/main.cpp
)

add_executable(${CMAKE_PROJECT_NAME} ${MY_SOURCES})

target_link_libraries(snake raylib)
