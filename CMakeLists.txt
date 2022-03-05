cmake_minimum_required(VERSION 3.5.1)

add_compile_options(-Wall -Wextra -Wno-unused-parameter)

project(j1939)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/config
)

add_subdirectory(config)
add_subdirectory(src)
add_subdirectory(module/googletest)
add_subdirectory(demo)
