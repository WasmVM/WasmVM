#ifndef WASMVM_DECODER_PARSEINSTR_DEF
#define WASMVM_DECODER_PARSEINSTR_DEF

#include <structures/WasmFunc.h>
#include <structures/instrs/Numeric.h>
#include <structures/instrs/Parametric.h>
#include <structures/instrs/Variable.h>
#include <structures/instrs/Memory.h>
#include <structures/instrs/Control.h>

int parseInstr(WasmFunc *func, uint8_t **read_p, const uint8_t *end_p);
WasmNumericInstr* parseNumericInstr(uint8_t opcode, uint8_t **read_p, const uint8_t *end_p);
WasmParametricInstr* parseParametricInstr(uint8_t opcode, uint8_t **read_p, const uint8_t *end_p);
WasmVariableInstr* parseVariableInstr(uint8_t opcode, uint8_t **read_p, const uint8_t *end_p);
WasmMemoryInstr* parseMemoryInstr(uint8_t opcode, uint8_t **read_p, const uint8_t *end_p);
WasmControlInstr* parseControlInstr(uint8_t opcode, uint8_t **read_p, const uint8_t *end_p);

#endif