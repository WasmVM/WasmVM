#ifndef WASMVM_VALIDATOR_VALIDATE_DEF
#define WASMVM_VALIDATOR_VALIDATE_DEF

#include <stdint.h>
#include <dataTypes/FuncType.h>
#include <dataTypes/list_p.h>
#include <dataTypes/vector_p.h>
#include <dataTypes/stack_p.h>
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

#include "Context.h"

int validate_Module(WasmModule* module);
int validate_FunctionType(FuncType funcType);
int validate_Func(WasmFunc* func, WasmModule* module);
int validate_Expr(list_p expr, Context* context);
int validate_Table(WasmTable* table);
int validate_Memory(WasmMemory* memory);
int validate_Global(WasmGlobal* global);
int validate_Elem(WasmElem* elem, WasmModule* module);
int validate_Data(WasmData* data, WasmModule* module);
int validate_Export(WasmExport* exports, WasmModule* module);
int validate_Import(WasmImport* imports, WasmModule* module);
int validate_Instr_const(WasmNumericInstr* instr, Context* context, stack_p opds, stack_p ctrls);
int validate_Instr_unop(WasmNumericInstr* instr, Context* context, stack_p opds, stack_p ctrls);
int validate_Instr_binop(WasmNumericInstr* instr, Context* context, stack_p opds, stack_p ctrls);
int validate_Instr_testop(WasmNumericInstr* instr, Context* context, stack_p opds, stack_p ctrls);
int validate_Instr_relop(WasmNumericInstr* instr, Context* context, stack_p opds, stack_p ctrls);
int validate_Instr_cvtop(WasmNumericInstr* instr, Context* context, stack_p opds, stack_p ctrls);
int validate_Instr_drop(WasmParametricInstr* instr, Context* context, stack_p opds, stack_p ctrls);
int validate_Instr_select(WasmParametricInstr* instr, Context* context, stack_p opds, stack_p ctrls);
int validate_Instr_get_local(WasmVariableInstr* instr, Context* context, stack_p opds, stack_p ctrls);
int validate_Instr_set_local(WasmVariableInstr* instr, Context* context, stack_p opds, stack_p ctrls);
int validate_Instr_tee_local(WasmVariableInstr* instr, Context* context, stack_p opds, stack_p ctrls);
int validate_Instr_get_global(WasmVariableInstr* instr, Context* context, stack_p opds, stack_p ctrls);
int validate_Instr_set_global(WasmVariableInstr* instr, Context* context, stack_p opds, stack_p ctrls);
int validate_Instr_load(WasmMemoryInstr* instr, Context* context, stack_p opds, stack_p ctrls);
int validate_Instr_loadN(WasmMemoryInstr* instr, Context* context, stack_p opds, stack_p ctrls);
int validate_Instr_store(WasmMemoryInstr* instr, Context* context, stack_p opds, stack_p ctrls);
int validate_Instr_storeN(WasmMemoryInstr* instr, Context* context, stack_p opds, stack_p ctrls);
int validate_Instr_memory_size(WasmMemoryInstr* instr, Context* context, stack_p opds, stack_p ctrls);
int validate_Instr_memory_grow(WasmMemoryInstr* instr, Context* context, stack_p opds, stack_p ctrls);
int validate_Instr_nop(WasmControlInstr* instr, Context* context, stack_p opds, stack_p ctrls);
int validate_Instr_unreachable(WasmControlInstr* instr, Context* context, stack_p opds, stack_p ctrls);
int validate_Instr_block(WasmControlInstr* instr, Context* context, stack_p opds, stack_p ctrls);
int validate_Instr_end(WasmControlInstr* instr, Context* context, stack_p opds, stack_p ctrls);
int validate_Instr_else(WasmControlInstr* instr, Context* context, stack_p opds, stack_p ctrls);
int validate_Instr_loop(WasmControlInstr* instr, Context* context, stack_p opds, stack_p ctrls);
int validate_Instr_if(WasmControlInstr* instr, Context* context, stack_p opds, stack_p ctrls);
int validate_Instr_br(WasmControlInstr* instr, Context* context, stack_p opds, stack_p ctrls);
int validate_Instr_br_if(WasmControlInstr* instr, Context* context, stack_p opds, stack_p ctrls);
int validate_Instr_br_table(WasmControlInstr* instr, Context* context, stack_p opds, stack_p ctrls);
int validate_Instr_return(WasmControlInstr* instr, Context* context, stack_p opds, stack_p ctrls);
int validate_Instr_call(WasmControlInstr* instr, Context* context, stack_p opds, stack_p ctrls);
int validate_Instr_call_indirect(WasmControlInstr* instr, Context* context, stack_p opds, stack_p ctrls);

#endif