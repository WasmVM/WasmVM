add_library(WasmVM_core SHARED
    decode/decode.c
    decode/sections.c
    decode/utils.c
    decode/parseFuncBody.c
    instance/free.c
    instance/module.c
    instantiate/instantiate.c
    instantiate/instrs.c
    invoke/func_invoke.c
    invoke/invoke.c
    invoke/exec.c
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

if(INSTALL_C)
install(TARGETS WasmVM_core
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)
endif(INSTALL_C)