target_sources(WasmVM_host PRIVATE
    sysenv.cpp
    sys_fs.cpp
    sys_proc.cpp
)
target_include_directories(WasmVM_host PRIVATE
    ${CMAKE_SOURCE_DIR}/src/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)
