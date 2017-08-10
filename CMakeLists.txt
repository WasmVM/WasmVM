cmake_minimum_required(VERSION 2.6)
project(Wasm_Interpret)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -g")
include_directories(src src/Instance src/Component src/Exception)
add_library( WAIntpCore
	src/Store.cpp
	src/Component/Loader.cpp
	src/Component/Util.cpp
	src/Component/Core.cpp
	src/Exception/Exception.cpp
	src/Exception/LoaderException.cpp
	src/Instance/LoaderInsts.cpp
)
add_executable(Wasm src/main.cpp src/Value.cpp src/Stack.cpp)
target_link_libraries(Wasm WAIntpCore)