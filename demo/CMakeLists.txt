set(BINARY ${CMAKE_PROJECT_NAME}_demo)
file(GLOB_RECURSE SOURCES LIST_DIRECTORIES false *.h *.cpp *.c)
set(SOURCES ${SOURCES})
add_executable(${BINARY} ${SOURCES})
add_test(NAME ${BINARY} COMMAND ${BINARY})

target_link_libraries(${BINARY} PUBLIC -Wl,--whole-archive ${CMAKE_PROJECT_NAME}_config ${CMAKE_PROJECT_NAME}_core -Wl,--no-whole-archive gtest)
