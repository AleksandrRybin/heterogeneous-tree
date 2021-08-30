cmake_minimum_required (VERSION 3.16)

project(heterogeneous-tree
        DESCRIPTION "Heterogeneous tree header only library"
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(tree_lib INTERFACE)
target_include_directories(tree_lib INTERFACE include)

add_executable(tree_demo src/tree_lib/demo.cpp)
target_link_libraries(tree_demo PUBLIC tree_lib)

find_package(Boost 1.65.0 REQUIRED COMPONENTS program_options)

set(COMPILE_OPTIONS -Wall -Werror -Wpedantic)
if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    list(APPEND COMPILE_OPTIONS -O3 -flto)
endif()

target_compile_options(tree_demo PUBLIC ${COMPILE_OPTIONS})
target_include_directories(tree_demo PRIVATE ${Boost_INCLUDE_DIRS})
target_link_libraries(tree_demo PRIVATE ${Boost_LIBRARIES})
