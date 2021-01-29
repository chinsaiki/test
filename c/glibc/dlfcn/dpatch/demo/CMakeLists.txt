cmake_minimum_required(VERSION 3.16)

project(
    dpatch_demos
    VERSION 0.0.0
    DESCRIPTION "Demonstration target programs `dpatch`."
    LANGUAGES C
)

add_executable(self_patch ${PROJECT_SOURCE_DIR}/self_patch.c)
add_library(charlie SHARED ${PROJECT_SOURCE_DIR}/charlie.c)

# `dpatch's` targets must expose all their symbols as dynamic symbols so
# `dpatch can manipulate them with `dlopen` and friends, as well as to
# easily generate backtraces for checking function activeness.
#
# These are limitations of the `libdl`, which we accept for speed of
# implementation. Hypothetically, a comparable loader-style patching
# tool could also use the static symbol stable in a program to avoid
# this limitation. We ignore this for now because we are focused on
# writing dynamic patching demonstrations, not run time dynamic
# linker-loaders.
set_property(TARGET self_patch PROPERTY ENABLE_EXPORTS 1)

configure_file(
    ${PROJECT_SOURCE_DIR}/self_patch.patch
    ${PROJECT_BINARY_DIR}/self_patch.patch
    COPYONLY
)

configure_file(
    ${PROJECT_SOURCE_DIR}/external_patch.patch
    ${PROJECT_BINARY_DIR}/external_patch.patch
    COPYONLY
)
install(
    TARGETS
        self_patch
    RUNTIME 
    DESTINATION
        dpatch/demo
)

install(
    TARGETS
        charlie
    LIBRARY
)

install(
    FILES
        ${PROJECT_BINARY_DIR}/self_patch.patch
        ${PROJECT_BINARY_DIR}/external_patch.patch
    TYPE SYSCONF
)

