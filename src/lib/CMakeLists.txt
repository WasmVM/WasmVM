add_subdirectory(core/runtime)

add_library(wasmvm_core
    Decoder.c
    core/Store.c
    core/Stack.c
    instance/ExportInst.c
    instance/FuncInst.c
    instance/GlobalInst.c
    instance/MemInst.c
    instance/ModuleInst.c
    instance/TableInst.c
    instance/ControlInstrInst.c
    dataTypes/stack.c
    dataTypes/Value.c
    dataTypes/vector.c
    dataTypes/FuncType.c
    $<TARGET_OBJECTS:wasmvm_runtime>
)

install(TARGETS wasmvm_core
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)