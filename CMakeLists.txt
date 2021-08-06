cmake_minimum_required(VERSION 3.16)
project(WasmVM)
set(PROJECT_ROOT ${CMAKE_CURRENT_LIST_DIR})
set(CMAKE_C_STANDARD 11)

# pthread
#set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
#find_package(Threads REQUIRED)

add_subdirectory(src)
add_subdirectory(third_party)
add_subdirectory(test)