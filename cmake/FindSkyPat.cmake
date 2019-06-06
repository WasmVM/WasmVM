find_library(SKYPAT_LIBRARIES
    NAMES skypat
    HINTS ${PROJECT_BINARY_DIR}/SkyPat/lib
)
get_filename_component(SKYPAT_LIBRARY_DIR ${SKYPAT_LIBRARIES} DIRECTORY)
find_path(SKYPAT_INCLUDE_DIR
    NAMES skypat/skypat.h
    HINTS ${PROJECT_BINARY_DIR}/SkyPat/include
)
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SKYPAT REQUIRED_VARS SKYPAT_INCLUDE_DIR SKYPAT_LIBRARY_DIR)