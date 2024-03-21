if(DEV_BUILD)
    set(CPACK_PACKAGE_NAME ${CMAKE_PROJECT_NAME}-dev)
else(DEV_BUILD)
    set(CPACK_PACKAGE_NAME ${CMAKE_PROJECT_NAME})
endif(DEV_BUILD)

if(WIN32 AND NOT UNIX)
    set(CPACK_SET_DESTDIR OFF)
    set(CPACK_NSIS_PACKAGE_NAME WasmVM)
    set(CPACK_NSIS_MODIFY_PATH ON)
    set(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
    set(CPACK_PACKAGE_INSTALL_DIRECTORY WasmVM)
else(WIN32 AND NOT UNIX)
    set(CPACK_SET_DESTDIR ON)
endif(WIN32 AND NOT UNIX)
set(CPACK_PACKAGE_VENDOR "WasmVM")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A WebAssembly virtual machine implementation")
set(CPACK_VERBATIM_VARIABLES True)
set(CPACK_OUTPUT_FILE_PREFIX ${PROJECT_ROOT}/packages)
set(CPACK_DEBIAN_PACKAGE_DEPENDS "")

set(CPACK_THREADS 0)

set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Luis Hsu")
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA
    ${CMAKE_CURRENT_LIST_DIR}/triggers
)
set(CPACK_DEBIAN_FILE_NAME DEB-DEFAULT)

include(CPack)

cpack_add_component(headers DISPLAY_NAME "C++ Headers"
  DESCRIPTION "C++ header files of WasmVM"
)
cpack_add_component(library DISPLAY_NAME "Static library"
  DESCRIPTION "WasmVM static library"
)
cpack_add_component(dll DISPLAY_NAME "Shared library"
  DESCRIPTION "WasmVM shared library"
)
cpack_add_component(modules DISPLAY_NAME "Host modules"
  DESCRIPTION "WasmVM host modules"
)
cpack_add_component(programs DISPLAY_NAME "Programs"
  DESCRIPTION "WasmVM program executables"
)