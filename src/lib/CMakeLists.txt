add_subdirectory(core/runtime)

add_library(wasmvm_core
    Decoder.c
    Loader.c
    core/Store.c
    core/Stack.c
    instance/ExportInst.c
    instance/FuncInst.c
    instance/GlobalInst.c
    instance/MemInst.c
    instance/ModuleInst.c
    instance/TableInst.c
    instance/ControlInstrInst.c
    instance/InstrInst.c
    instance/MemoryInstrInst.c
    instance/NumericInstrInst.c
    instance/ParametricInstrInst.c
    instance/VariableInstrInst.c
    dataTypes/stack.c
    dataTypes/Value.c
    dataTypes/vector.c
    dataTypes/list.c
    dataTypes/queue.c
    dataTypes/FuncType.c
    structures/WasmData.c
    structures/WasmElem.c
    structures/WasmFunc.c
    structures/WasmModule.c
    validator/Validator.c
    validator/Validates.c
    validator/Context.c
    validator/ExprValidate.c
    validator/InstrValidates.c
    $<TARGET_OBJECTS:wasmvm_runtime>
)

target_link_libraries(wasmvm_core
    ${CMAKE_THREAD_LIBS_INIT}
)

install(TARGETS wasmvm_core
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)