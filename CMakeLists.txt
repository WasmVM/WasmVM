cmake_minimum_required(VERSION 3.5)
project(WasmVM)
set(PROJECT_ROOT ${CMAKE_CURRENT_LIST_DIR})
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c11")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS_DEBUG} -std=c++11")

add_subdirectory(src)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(PROJECT_TEST_DIR ${CMAKE_CURRENT_LIST_DIR}/test)
    include(CTest)

    add_compile_options("-g")

    find_package(SkyPat REQUIRED)
    include_directories(${SKYPAT_INCLUDE_DIR})
    add_subdirectory(test)
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")