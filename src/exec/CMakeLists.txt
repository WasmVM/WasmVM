add_executable(wasmvm-as
    as.cpp
    CommandParser.cpp
)
target_link_libraries(wasmvm-as
    WasmVM
)

add_executable(wasmvm-ld
    ld.cpp
    CommandParser.cpp
    Linker.cpp
    Archive.cpp
)
target_link_libraries(wasmvm-ld
    WasmVM
)

add_executable(readwasm
    readwasm.cpp
    CommandParser.cpp
)
target_link_libraries(readwasm
    WasmVM
)

add_executable(wasmvm-ar
    ar.cpp
    CommandParser.cpp
    Archive.cpp
)
target_link_libraries(wasmvm-ar
    WasmVM
)

if(UNIX)
    set(DEFAULT_MODULE_PATH "/usr/local/lib" CACHE PATH "Default module lookup path")
elseif(WIN32)
    set(DEFAULT_MODULE_PATH "C:\\Program Files\\${PROJECT_NAME}\\lib" CACHE PATH "Default module lookup path")
endif(UNIX)

add_executable(wasmvm
    wasmvm.cpp
    CommandParser.cpp
    ModuleQueue.cpp
)
target_link_libraries(wasmvm
    WasmVM
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