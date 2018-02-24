cmake_minimum_required(VERSION 3.5)

list(INSERT CMAKE_MODULE_PATH 0 ${CMAKE_CURRENT_LIST_DIR}/cmake)
list(INSERT CMAKE_PREFIX_PATH 0 ${CMAKE_CURRENT_LIST_DIR}/cmake)

include(AddModule)
include(AddExternalModule)
include(SetupCatch2)

enable_testing()

add_module(NAME immutable-vector)
add_module(NAME immutable-vector_test)

add_module(NAME immutable-optional)
add_module(NAME immutable-optional_test)

add_module(NAME wrapper)
add_module(NAME wrapper_test)

add_module(NAME immutable)

add_subdirectory(doc)
