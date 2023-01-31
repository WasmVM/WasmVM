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

add_library(WasmVM_obj OBJECT
    parse/parse.cpp
    parse/token.cpp
    parse/nodes/module.cpp
    parse/nodes/Type.cpp
    parse/nodes/FuncType.cpp
    parse/nodes/ValueType.cpp
    parse/nodes/Import.cpp
    dump/dump.cpp
    dump/type.cpp
    dump/import.cpp
    Types.cpp
)
set_property(TARGET WasmVM_obj PROPERTY POSITION_INDEPENDENT_CODE 1)

add_library(WasmVM SHARED $<TARGET_OBJECTS:WasmVM_obj>)
add_library(WasmVM_static STATIC $<TARGET_OBJECTS:WasmVM_obj>)

if(INSTALL_C)
install(TARGETS WasmVM_core
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)
endif(INSTALL_C)