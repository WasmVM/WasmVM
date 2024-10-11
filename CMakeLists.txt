cmake_minimum_required(VERSION 3.16)
project(WasmVM)

set(CMAKE_PROJECT_VERSION_MAJOR 1)
set(CMAKE_PROJECT_VERSION_MINOR 2)
set(CMAKE_PROJECT_VERSION_PATCH 2)

set(PROJECT_ROOT ${CMAKE_CURRENT_LIST_DIR})
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED 11)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED 20)

if(MSVC)
  add_compile_options(/bigobj)
endif(MSVC)

include_directories(${CMAKE_CURRENT_LIST_DIR}/src/include)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

if(UNIX)
  set(DEFAULT_MODULE_PATH "/usr/local/lib" CACHE PATH "Default module lookup path")
elseif(WIN32)
  set(DEFAULT_MODULE_PATH "C:\\Program Files\\${PROJECT_NAME}\\lib" CACHE PATH "Default module lookup path")
endif(UNIX)

set(CMAKE_INSTALL_RPATH ${DEFAULT_MODULE_PATH})
set(DEV_BUILD ON CACHE BOOL "Build & install static library and headers")

set(CONSOLE_COLOR ON CACHE BOOL "Turn on color in console result")
if(CONSOLE_COLOR)
  add_compile_definitions(CONSOLE_COLOR)
endif(CONSOLE_COLOR)

set(BUILD_HOST_MODULES ON CACHE BOOL "Build host modules of WasmVM")
if(BUILD_HOST_MODULES)
  add_subdirectory(host)
  add_compile_definitions(HOST_MODULES)
endif(BUILD_HOST_MODULES)

link_directories(${PROJECT_BINARY_DIR}/external/antlr4_runtime/src/antlr4_runtime-install/lib)
add_subdirectory(external)
add_subdirectory(src)

set(BUILD_DOCS ON CACHE BOOL "Build documentation")
if(BUILD_DOCS)
  add_subdirectory(docs)
endif(BUILD_DOCS)

set(BUILD_TESTS ON CACHE BOOL "Build tests")
if(BUILD_TESTS)
  include(CTest)
  enable_testing()
  # add_subdirectory(test)
endif(BUILD_TESTS)

include(pack)