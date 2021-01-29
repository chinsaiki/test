set(MYLIB_UNIT_TESTS_ROOT ${CMAKE_SOURCE_DIR}/tests/my_lib)

include_directories(
    ${CMAKE_SOURCE_DIR}/src
)

set(MYLIB_UNIT_TESTS_SOURCE_FILES
    ${MYLIB_UNIT_TESTS_ROOT}/testA.cpp
    ${MYLIB_UNIT_TESTS_ROOT}/testRadio.cpp
    ${MYLIB_UNIT_TESTS_ROOT}/testMicrowave.cpp
    ${MYLIB_UNIT_TESTS_ROOT}/testSwitcher.cpp
    ${MYLIB_UNIT_TESTS_ROOT}/SwitchableMock.c
)