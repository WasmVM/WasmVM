add_subdirectory(lib)
add_subdirectory(exec)

if(DEV_BUILD)
install(DIRECTORY include/
    DESTINATION include/WasmVM
    FILES_MATCHING PATTERN "*.hpp"
)
endif(DEV_BUILD)