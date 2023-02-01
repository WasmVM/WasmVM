cmake_minimum_required(VERSION 3.16)
project(WasmVM)
set(PROJECT_ROOT ${CMAKE_CURRENT_LIST_DIR})
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 20)

include_directories(${CMAKE_CURRENT_LIST_DIR}/src/include)

set(INSTALL_C ON CACHE BOOL "Install C implementations")
set(INSTALL_CPP ON CACHE BOOL "Install C++ implementations")

# set(CONSOLE_COLOR ON CACHE BOOL "Apply color to console outputs")
# if(CONSOLE_COLOR)
#     set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-DCONSOLE_COLOR")
# endif(CONSOLE_COLOR)

set(BUILD_HOST_MODULES ON CACHE BOOL "Build host modules of WasmVM")
add_subdirectory(src)

set(BUILD_HOST_MODULES ON CACHE BOOL "Build host modules of WasmVM")
if(BUILD_HOST_MODULES)
    add_subdirectory(host)
endif(BUILD_HOST_MODULES)

set(BUILD_TESTS ON CACHE BOOL "Build tests")
if(BUILD_TESTS)
    add_subdirectory(third_party)
    include(CTest)
    enable_testing()
    add_subdirectory(test)
endif(BUILD_TESTS)