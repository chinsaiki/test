cmake_minimum_required(VERSION 3.16)

project(
    dpatch_library
    VERSION 0.0.0
    DESCRIPTION "Experimental dynamic software updating framework."
    LANGUAGES C
)

set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)

add_library(dpatch SHARED
    ${PROJECT_SOURCE_DIR}/main.c
    ${PROJECT_SOURCE_DIR}/x64_code_generator.c
    ${PROJECT_SOURCE_DIR}/machine_code.c
    ${PROJECT_SOURCE_DIR}/patch_script.c
    ${PROJECT_SOURCE_DIR}/patch_set.c
    ${PROJECT_SOURCE_DIR}/patch.c
    ${PROJECT_SOURCE_DIR}/status.c
)

set_property(TARGET dpatch PROPERTY C_STANDARD 99)

target_link_libraries(dpatch PRIVATE ${CMAKE_DL_LIBS} Threads::Threads)

target_include_directories(dpatch PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_compile_definitions(dpatch PRIVATE _GNU_SOURCE)

target_compile_options(
    dpatch PRIVATE
    "SHELL:-W"
    "SHELL:-Wall"
    "SHELL:-Wextra"
    "SHELL:-Werror"
    "SHELL:-pedantic"
)

install(TARGETS dpatch LIBRARY)
