cmake_minimum_required(VERSION 3.16)
project(WasmVM)
set(PROJECT_ROOT ${CMAKE_CURRENT_LIST_DIR})
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 20)

include_directories(${CMAKE_CURRENT_LIST_DIR}/src/include)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(DEV_BUILD ON CACHE BOOL "Build & install static library and headers")

set(CONSOLE_COLOR ON CACHE BOOL "Turn on color in console result")
if(CONSOLE_COLOR)
  add_compile_definitions(CONSOLE_COLOR)
endif(CONSOLE_COLOR)

add_subdirectory(src)

set(BUILD_HOST_MODULES ON CACHE BOOL "Build host modules of WasmVM")
if(BUILD_HOST_MODULES)
  add_subdirectory(host)
  add_compile_definitions(HOST_MODULES)
endif(BUILD_HOST_MODULES)

set(BUILD_TESTS ON CACHE BOOL "Build tests")
if(BUILD_TESTS)
  include(CTest)
  enable_testing()
  add_subdirectory(test)
endif(BUILD_TESTS)

include(pack_deb)