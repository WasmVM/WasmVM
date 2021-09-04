#ifndef WASMVM_DECODE_PARSEFUNCBODY_DEF
#define WASMVM_DECODE_PARSEFUNCBODY_DEF

#include <defines.h>
#include <structures/WasmFunc.h>

int parseFuncBody(WasmFunc* const func, const byte_t **read_p, const byte_t *end_p);
// WasmNumericInstr* parseNumericInstr(uint8_t opcode, uint8_t **read_p, const uint8_t *end_p);
// WasmParametricInstr* parseParametricInstr(uint8_t opcode, uint8_t **read_p, const uint8_t *end_p);
// WasmVariableInstr* parseVariableInstr(uint8_t opcode, uint8_t **read_p, const uint8_t *end_p);
// WasmMemoryInstr* parseMemoryInstr(uint8_t opcode, uint8_t **read_p, const uint8_t *end_p);

#endif