include_directories(${CMAKE_CURRENT_LIST_DIR}/include)

add_subdirectory(lib)
add_subdirectory(exec)

install(DIRECTORY include
    DESTINATION include/WasmVM
    FILES_MATCHING PATTERN "*.h"
)