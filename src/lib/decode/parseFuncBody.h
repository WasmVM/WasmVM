#ifndef WASMVM_DECODE_PARSEFUNCBODY_DEF
#define WASMVM_DECODE_PARSEFUNCBODY_DEF

#include <defines.h>
#include <structures/WasmFunc.h>

int parseFuncBody(WasmFunc* const func, const byte_t **read_p, const byte_t *end_p);

#endif