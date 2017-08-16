cmake_minimum_required(VERSION 2.6)
project(WasmVM)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Werror")
set(CMAKE_DEBUG_POSTFIX "_d")
option(USE_SYSCALL "Whether use WasmVM syscall or not" OFF)
if(USE_SYSCALL)
add_definitions(-DUSE_SYSCALL)
endif(USE_SYSCALL)
include_directories(src src/Instance src/Component src/Exception src/Instruction)
add_library( WasmVM_Core
	src/Value.cpp
	src/Stack.cpp
	src/Store.cpp
	src/Component/Loader.cpp
	src/Component/Util.cpp
	src/Component/Core.cpp
	src/Component/Decoder.cpp
	src/Exception/Exception.cpp
	src/Exception/LoaderException.cpp
	src/Instance/LoaderInsts.cpp
	src/Instruction/ControlInstr.cpp
	src/Instruction/NumericInstr.cpp
	src/Instruction/MemoryInstr.cpp
)
add_executable(WasmVM src/main.cpp)
target_link_libraries(WasmVM WasmVM_Core)