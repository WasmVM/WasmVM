cmake_minimum_required(VERSION 2.6)
project(WasmVM)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -lm")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -g")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Werror")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_DEBUG_POSTFIX "_d")
option(USE_SYSCALL "Whether use WasmVM syscall or not" ON)

if(USE_SYSCALL)
add_definitions(-DUSE_SYSCALL)
include_directories(src/Syscall)
add_subdirectory(src/Syscall)
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
	src/Instruction/ParametricInstr.cpp
	src/Instruction/VariableInstr.cpp
)
add_executable(WasmVM src/main.cpp)
target_link_libraries(WasmVM WasmVM_Core)
if(USE_SYSCALL)
target_link_libraries(WasmVM WasmVM_Syscall)
endif(USE_SYSCALL)