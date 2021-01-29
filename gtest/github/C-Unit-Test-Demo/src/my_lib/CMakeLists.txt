cmake_minimum_required(VERSION 3.5)
project(unit_test_demo)


# Configure a library that contains all C modules
set(HEADERS
    A.h
    Microwave.h
    Radio.h
    Switchable.h
    Switcher.h
    )

set(SOURCES 
    A.c
    Microwave.c
    Radio.c
    Switcher.c
    )

add_library(my_lib ${HEADERS} ${SOURCES})
set_target_properties(my_lib PROPERTIES LINKER_LANGUAGE C)
