add_subdirectory(lib)
# add_subdirectory(exec)

install(DIRECTORY include/
    TYPE INCLUDE
    FILES_MATCHING PATTERN "*.h"
)