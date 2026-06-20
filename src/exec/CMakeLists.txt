# On Windows the WasmVM DLL exports no data symbols, so executables that touch
# header-inline statics fail to link against it. Link the static library there;
# elsewhere link the shared library as before.
if(WIN32)
    set(WASMVM_LINK_LIB WasmVM_static)
else()
    set(WASMVM_LINK_LIB WasmVM)
endif()

add_executable(wasmvm-as
    as.cpp
    CommandParser.cpp
)
target_link_libraries(wasmvm-as
    ${WASMVM_LINK_LIB}
)

add_executable(wasmvm-ld
    ld.cpp
    CommandParser.cpp
    Linker.cpp
    Archive.cpp
)
target_link_libraries(wasmvm-ld
    ${WASMVM_LINK_LIB}
)

add_executable(readwasm
    readwasm.cpp
    CommandParser.cpp
)
target_link_libraries(readwasm
    ${WASMVM_LINK_LIB}
)

add_executable(wasmvm-ar
    ar.cpp
    CommandParser.cpp
    Archive.cpp
)
target_link_libraries(wasmvm-ar
    ${WASMVM_LINK_LIB}
)

add_executable(wasmvm
    wasmvm.cpp
    CommandParser.cpp
    ModuleQueue.cpp
)
target_link_libraries(wasmvm
    ${WASMVM_LINK_LIB}
)
if(BUILD_HOST_MODULES)
target_link_libraries(wasmvm
    WasmVM_host
)
endif(BUILD_HOST_MODULES)

target_compile_definitions(wasmvm PUBLIC DEFAULT_MODULE_PATH="${DEFAULT_MODULE_PATH}")

install(TARGETS wasmvm-as readwasm wasmvm wasmvm-ar wasmvm-ld
    RUNTIME DESTINATION bin
    COMPONENT programs
)