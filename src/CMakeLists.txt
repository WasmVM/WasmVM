add_subdirectory(lib)

if(INSTALL_C)
install(DIRECTORY include/
    TYPE INCLUDE
    FILES_MATCHING PATTERN "*.h"
)
endif(INSTALL_C)

if(INSTALL_CPP)
install(DIRECTORY include/
    TYPE INCLUDE
    FILES_MATCHING PATTERN "*.hpp"
)
endif(INSTALL_CPP)