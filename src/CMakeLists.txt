include_directories(${CMAKE_CURRENT_LIST_DIR}/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/lib)

add_subdirectory(lib)
add_subdirectory(exec)

install(DIRECTORY include
    DESTINATION include/WasmVM
    FILES_MATCHING PATTERN "*.h"
)