# target_link_libraries(wasmvm_core
#     ${CMAKE_THREAD_LIBS_INIT}
#     m
# )

# install(TARGETS wasmvm_core
#     ARCHIVE DESTINATION lib
#     LIBRARY DESTINATION lib
# )

add_library(WasmVM
    decode/decode.c
    decode/sections.c
    decode/utils.c
    decode/parseFuncBody.c
    instance/free.c
    instance/module.c
    instantiate/instantiate.c
    instantiate/instrs.c
    invoke/invoke.c
    error.c
    defines.c
    free.c
    store.c
    import.c
    func.c
    table.c
    mem.c
    global.c
    instance.c
)