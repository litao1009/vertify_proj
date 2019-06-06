cmake_minimum_required (VERSION 3.8)

project (gl3w)

file(GLOB_RECURSE SRC_LIST ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c*)
include_directories(include)

add_library(gl3w STATIC ${SRC_LIST})

if (UNIX)
target_link_libraries(gl3w dl)
endif()

set(gl3w_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include PARENT_SCOPE)