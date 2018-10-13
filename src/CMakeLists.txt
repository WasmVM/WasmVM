include_directories(${CMAKE_CURRENT_LIST_DIR}/include)

add_subdirectory(lib)

install(DIRECTORY include
    DESTINATION include/WasmVM
    FILES_MATCHING PATTERN "*.h"
)