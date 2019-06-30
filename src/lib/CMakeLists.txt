add_subdirectory(core/runtime)

add_library(wasmvm_core
    Executor.c
    Loader.c
    LoaderRequest.c
    core/Store.c
    core/Stack.c
    core/Core.c
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
    dataTypes/stack_p.c
    dataTypes/Value.c
    dataTypes/vector_p.c
    dataTypes/list_p.c
    dataTypes/queue_p.c
    dataTypes/Label.c
    dataTypes/FuncType.c
    dataTypes/Frame.c
    structures/instrs/Control.c
    structures/instrs/Parametric.c
    structures/instrs/Numeric.c
    structures/instrs/Memory.c
    structures/instrs/Variable.c
    structures/instrs/WasmInstr.c
    structures/WasmData.c
    structures/WasmElem.c
    structures/WasmFunc.c
    structures/WasmModule.c
    structures/WasmExport.c
    structures/WasmImport.c
    stages/validator/Validator.c
    stages/validator/Validates.c
    stages/validator/Context.c
    stages/validator/ExprValidate.c
    stages/validator/InstrValidates.c
    stages/instanciator/Instanciator.c
    stages/instanciator/Allocates.c
    stages/instanciator/InstrAllocates.c
    stages/decoder/Decoder.c
    stages/decoder/sections.c
    stages/decoder/parseInstr.c
    stages/decoder/Utils.c
    $<TARGET_OBJECTS:wasmvm_runtime>
)

target_link_libraries(wasmvm_core
    ${CMAKE_THREAD_LIBS_INIT}
    m
)

install(TARGETS wasmvm_core
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)
