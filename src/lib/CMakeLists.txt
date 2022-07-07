add_library(WasmVM
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
    validate/validate.c
    validate/func.c
    validate/stack.c
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

install(TARGETS WasmVM
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)