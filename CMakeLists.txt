cmake_minimum_required(VERSION 3.16)
project(WasmVM)
set(PROJECT_ROOT ${CMAKE_CURRENT_LIST_DIR})
set(CMAKE_C_STANDARD 11)

add_subdirectory(src)
add_subdirectory(third_party)

set(MEMORYCHECK_COMMAND_OPTIONS "--error-exitcode=1")
include(CTest)
enable_testing()
add_subdirectory(test)