cmake_minimum_required(VERSION 3.16)
project(WasmVM)
set(PROJECT_ROOT ${CMAKE_CURRENT_LIST_DIR})
set(CMAKE_C_STANDARD 11)

include_directories(${CMAKE_CURRENT_LIST_DIR}/src/include)

add_subdirectory(src)
add_subdirectory(host)
add_subdirectory(third_party)

set(BUILD_TESTS ON CACHE BOOL "Build tests")
if(BUILD_TESTS)
    include(CTest)
    enable_testing()
    add_subdirectory(test)
endif(BUILD_TESTS)