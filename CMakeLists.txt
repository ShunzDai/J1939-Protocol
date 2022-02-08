cmake_minimum_required(VERSION 3.5.1)

project(J1939)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -no-pie")
set(CMAKE_BUILD_TYPE "Debug")

# Add header file include directories
include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  ${CMAKE_CURRENT_SOURCE_DIR}/config
)

file(GLOB_RECURSE DIR_SRCS
  src/*.c
  config/*.c
)

add_executable(demo1 ${DIR_SRCS} demo/j1939_demo1.c)

# target_link_libraries(demo pthread)
