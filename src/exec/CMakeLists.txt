add_executable(wasmvm-cat
    cat.cpp
)
target_link_libraries(wasmvm-cat
    WasmVM
)