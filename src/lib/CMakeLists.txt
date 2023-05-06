add_library(WasmVM_textfmt OBJECT
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
    dump/data.cpp
    dump/global.cpp
)
target_compile_options(WasmVM_textfmt PRIVATE -fPIC)

add_library(WasmVM_binfmt OBJECT
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
    decode/decode.cpp
)
target_compile_options(WasmVM_binfmt PRIVATE -fPIC)

add_library(WasmVM_src OBJECT
    exception.cpp
    functype.cpp
)
target_compile_options(WasmVM_src PRIVATE -fPIC)

add_library(WasmVM SHARED 
    $<TARGET_OBJECTS:WasmVM_textfmt>
    $<TARGET_OBJECTS:WasmVM_binfmt>
    $<TARGET_OBJECTS:WasmVM_src>
)
set_property(TARGET WasmVM PROPERTY POSITION_INDEPENDENT_CODE 1)
target_compile_options(WasmVM PRIVATE -fPIC)

add_library(WasmVM_static STATIC 
    $<TARGET_OBJECTS:WasmVM_textfmt>
    $<TARGET_OBJECTS:WasmVM_binfmt>
    $<TARGET_OBJECTS:WasmVM_src>
)
set_property(TARGET WasmVM_static PROPERTY OUTPUT_NAME WasmVM)

install(TARGETS WasmVM WasmVM_static 
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)