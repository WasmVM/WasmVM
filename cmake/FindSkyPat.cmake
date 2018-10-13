find_library(SKYPAT_LIBRARIES
    NAMES skypat
    HINTS ${SKYPAT_LIBRARY_DIR}
)
find_path(SKYPAT_INCLUDE_DIR
    NAMES skypat/skypat.h
)
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SKYPAT REQUIRED_VARS SKYPAT_INCLUDE_DIR SKYPAT_LIBRARIES)
