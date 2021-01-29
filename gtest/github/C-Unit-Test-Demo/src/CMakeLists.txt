cmake_minimum_required(VERSION 3.5)
project(unit_test_demo)

add_subdirectory(my_lib)


# Configure the executable that links in my_lib
add_executable(runMain main.c)
target_link_libraries(runMain my_lib)