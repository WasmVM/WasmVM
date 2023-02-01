add_subdirectory(lib)

if(INSTALL_C)
install(DIRECTORY include/
    DESTINATION include/WasmVM
    FILES_MATCHING PATTERN "*.h"
)
endif(INSTALL_C)

if(INSTALL_CPP)
install(DIRECTORY include/
    DESTINATION include/WasmVM
    FILES_MATCHING PATTERN "*.hpp"
)
endif(INSTALL_CPP)