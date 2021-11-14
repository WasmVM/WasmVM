add_library(wasmvm_spectest
    spectest.c
)
target_link_libraries(wasmvm_spectest
    WasmVM
)