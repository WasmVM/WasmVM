cmake_minimum_required(VERSION 3.5)
project(WasmVM)
set(PROJECT_ROOT ${CMAKE_CURRENT_LIST_DIR})
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)

# pthread
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
find_package(Threads REQUIRED)

add_subdirectory(src)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(PROJECT_TEST_DIR ${CMAKE_CURRENT_LIST_DIR}/test)
    include(CTest)

    add_compile_options("-g")

    include(ExternalProject)
    set($ENV{SHELL} "/bin/bash")

    find_package(SkyPat)
    if(NOT (SKYPAT_INCLUDE_DIR AND SKYPAT_LIBRARY_DIR))
        ExternalProject_Add(SkyPat
            PREFIX SkyPat
            GIT_REPOSITORY https://github.com/skymizer/SkyPat.git
            GIT_TAG v3.1.1
            CONFIGURE_COMMAND "<SOURCE_DIR>/configure" "--prefix=<INSTALL_DIR>"
        )
        ExternalProject_Add_Step(SkyPat autogen
            COMMAND "<SOURCE_DIR>/autogen.sh"
            DEPENDEES download
            DEPENDERS configure
        )
        set(SKYPAT_INCLUDE_DIR ${PROJECT_BINARY_DIR}/SkyPat/include)
        set(SKYPAT_LIBRARY_DIR ${PROJECT_BINARY_DIR}/SkyPat/lib)
    endif(NOT (SKYPAT_INCLUDE_DIR AND SKYPAT_LIBRARY_DIR))
    include_directories(${SKYPAT_INCLUDE_DIR})
    link_directories(${SKYPAT_LIBRARY_DIR})

    add_subdirectory(test)
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")