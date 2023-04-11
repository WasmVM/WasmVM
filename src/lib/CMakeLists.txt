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
    parse/Token.cpp
    parse/nodes/Module.cpp
    parse/nodes/Type.cpp
    parse/nodes/FuncType.cpp
    parse/nodes/ValueType.cpp
    parse/nodes/Import.cpp
    parse/nodes/TableType.cpp
    parse/nodes/MemType.cpp
    parse/nodes/GlobalType.cpp
    parse/nodes/Func.cpp
    parse/nodes/Table.cpp
    parse/nodes/Elem.cpp
    parse/nodes/TypeUse.cpp
    parse/nodes/instruction.cpp
    parse/nodes/instruction/numeric.cpp
    parse/nodes/instruction/const.cpp
    parse/nodes/instruction/control.cpp
    parse/nodes/instruction/reference.cpp
    parse/nodes/instruction/parametric.cpp
    parse/nodes/instruction/variable.cpp
    parse/nodes/instruction/table.cpp
    parse/nodes/instruction/memory.cpp
    dump/dump.cpp
    dump/type.cpp
    dump/import.cpp
    dump/func.cpp
    dump/instr.cpp
    dump/export.cpp
    dump/elem.cpp
    exception.cpp
)
set_property(TARGET WasmVM_obj PROPERTY POSITION_INDEPENDENT_CODE 1)

add_library(WasmVM SHARED $<TARGET_OBJECTS:WasmVM_obj>)
add_library(WasmVM_static STATIC $<TARGET_OBJECTS:WasmVM_obj>)
set_property(TARGET WasmVM_static PROPERTY OUTPUT_NAME WasmVM)

if(INSTALL_C)
install(TARGETS WasmVM_core
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)
endif(INSTALL_C)

if(INSTALL_CPP)
install(TARGETS WasmVM WasmVM_static 
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)
endif(INSTALL_CPP)