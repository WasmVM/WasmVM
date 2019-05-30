add_executable(wasmvm
    wasmvm/main.c
)

target_link_libraries(wasmvm
    wasmvm_core
)

install(TARGETS wasmvm
    RUNTIME DESTINATION bin
)