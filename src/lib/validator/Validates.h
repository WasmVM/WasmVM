#ifndef WASMVM_VALIDATOR_VALIDATE_DEF
#define WASMVM_VALIDATOR_VALIDATE_DEF

#include <stdint.h>
#include <dataTypes/FuncType.h>
#include <dataTypes/vector.h>
#include <structures/WasmModule.h>
#include <structures/WasmFunc.h>
#include <structures/WasmTable.h>
#include <structures/WasmMemory.h>
#include <structures/WasmGlobal.h>
#include <structures/WasmElem.h>
#include <structures/WasmData.h>
#include <structures/WasmExport.h>
#include <structures/WasmImport.h>
#include <structures/instrs/WasmInstr.h>
#include <structures/instrs/Numeric.h>
#include <structures/instrs/Parametric.h>
#include <structures/instrs/Variable.h>
#include <structures/instrs/Memory.h>
#include <structures/instrs/Control.h>

int validate_Module(WasmModule* module);
int validate_FunctionType(FuncType* funcType);
int validate_Func(WasmFunc* func);
int validate_Expr(vector* expr);
int validate_Table(WasmTable* table);
int validate_Memory(WasmMemory* memory);
int validate_Global(WasmGlobal* global);
int validate_Elem(WasmElem* elem);
int validate_Data(WasmData* data);
int validate_Export(WasmExport* exports);
int validate_Import(WasmImport* imports);
int validate_Instr_const(WasmNumericInstr* instr);
int validate_Instr_unop(WasmNumericInstr* instr);
int validate_Instr_binop(WasmNumericInstr* instr);
int validate_Instr_testop(WasmNumericInstr* instr);
int validate_Instr_relop(WasmNumericInstr* instr);
int validate_Instr_cvtop(WasmNumericInstr* instr);
int validate_Instr_drop(WasmParametricInstr* instr);
int validate_Instr_select(WasmParametricInstr* instr);
int validate_Instr_get_local(WasmVariableInstr* instr);
int validate_Instr_set_local(WasmVariableInstr* instr);
int validate_Instr_tee_local(WasmVariableInstr* instr);
int validate_Instr_get_global(WasmVariableInstr* instr);
int validate_Instr_set_global(WasmVariableInstr* instr);
int validate_Instr_load(WasmMemoryInstr* instr);
int validate_Instr_loadN(WasmMemoryInstr* instr);
int validate_Instr_store(WasmMemoryInstr* instr);
int validate_Instr_storeN(WasmMemoryInstr* instr);
int validate_Instr_memory_size(WasmMemoryInstr* instr);
int validate_Instr_memory_grow(WasmMemoryInstr* instr);
int validate_Instr_nop(WasmControlInstr* instr);
int validate_Instr_unreachable(WasmControlInstr* instr);
int validate_Instr_block(WasmControlInstr* instr);
int validate_Instr_loop(WasmControlInstr* instr);
int validate_Instr_if(WasmControlInstr* instr);
int validate_Instr_br(WasmControlInstr* instr);
int validate_Instr_br_if(WasmControlInstr* instr);
int validate_Instr_br_table(WasmControlInstr* instr);
int validate_Instr_return(WasmControlInstr* instr);
int validate_Instr_call(WasmControlInstr* instr);
int validate_Instr_call_indirect(WasmControlInstr* instr);

#endif