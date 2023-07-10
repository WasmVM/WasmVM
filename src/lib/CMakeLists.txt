add_library(WasmVM_textfmt OBJECT
    # parse
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
    parse/nodes/Memory.cpp
    parse/nodes/Elem.cpp
    parse/nodes/Data.cpp
    parse/nodes/TypeUse.cpp
    parse/nodes/Global.cpp
    parse/nodes/Export.cpp
    parse/nodes/Start.cpp
    parse/nodes/instruction.cpp
    parse/nodes/instruction/numeric.cpp
    parse/nodes/instruction/i32.cpp
    parse/nodes/instruction/i64.cpp
    parse/nodes/instruction/f32.cpp
    parse/nodes/instruction/f64.cpp
    parse/nodes/instruction/const.cpp
    parse/nodes/instruction/control.cpp
    parse/nodes/instruction/reference.cpp
    parse/nodes/instruction/parametric.cpp
    parse/nodes/instruction/variable.cpp
    parse/nodes/instruction/table.cpp
    parse/nodes/instruction/memory.cpp
    parse/nodes/post_process.cpp
    # dump
    dump/dump.cpp
    dump/type.cpp
    dump/import.cpp
    dump/func.cpp
    dump/instr.cpp
    dump/export.cpp
    dump/elem.cpp
    dump/data.cpp
    dump/global.cpp
)
set_property(TARGET WasmVM_textfmt PROPERTY POSITION_INDEPENDENT_CODE 1)

add_library(WasmVM_binfmt OBJECT
    # encode
    encode/encode.cpp
    encode/type.cpp
    encode/import.cpp
    encode/table.cpp
    encode/global.cpp
    encode/func.cpp
    encode/memory.cpp
    encode/instr.cpp
    encode/export.cpp
    encode/elem.cpp
    encode/data.cpp
    # decode
    decode/decode.cpp
    decode/type.cpp
    decode/import.cpp
    decode/table.cpp
    decode/global.cpp
    decode/func.cpp
    decode/memory.cpp
    decode/instr.cpp
    decode/export.cpp
    decode/start.cpp
    decode/elem.cpp
    decode/data.cpp
)
set_property(TARGET WasmVM_binfmt PROPERTY POSITION_INDEPENDENT_CODE 1)

add_library(WasmVM_src OBJECT
    exception.cpp
    functype.cpp
)
set_property(TARGET WasmVM_src PROPERTY POSITION_INDEPENDENT_CODE 1)

add_library(WasmVM_validate OBJECT
    validate/validate.cpp
    validate/func.cpp
    validate/state.cpp
    validate/table.cpp
    validate/mem.cpp
    validate/global.cpp
    validate/elem.cpp
    validate/data.cpp
    validate/export.cpp
    validate/import.cpp
)
set_property(TARGET WasmVM_validate PROPERTY POSITION_INDEPENDENT_CODE 1)

add_library(WasmVM_exec OBJECT
    instanciate.cpp
    invoke/invoke.cpp
    invoke/control.cpp
    invoke/const.cpp
)
set_property(TARGET WasmVM_exec PROPERTY POSITION_INDEPENDENT_CODE 1)

add_library(WasmVM SHARED 
    $<TARGET_OBJECTS:WasmVM_textfmt>
    $<TARGET_OBJECTS:WasmVM_binfmt>
    $<TARGET_OBJECTS:WasmVM_src>
    $<TARGET_OBJECTS:WasmVM_validate>
    $<TARGET_OBJECTS:WasmVM_exec>
)
set_property(TARGET WasmVM PROPERTY POSITION_INDEPENDENT_CODE 1)

if(DEV_BUILD)
add_library(WasmVM_static STATIC 
    $<TARGET_OBJECTS:WasmVM_textfmt>
    $<TARGET_OBJECTS:WasmVM_binfmt>
    $<TARGET_OBJECTS:WasmVM_src>
    $<TARGET_OBJECTS:WasmVM_validate>
    $<TARGET_OBJECTS:WasmVM_exec>
)
set_property(TARGET WasmVM_static PROPERTY OUTPUT_NAME WasmVM)

install(TARGETS WasmVM_static 
    ARCHIVE DESTINATION lib
)
endif(DEV_BUILD)

install(TARGETS WasmVM
    LIBRARY DESTINATION lib
)