#ifndef WASMVM_TEST_HELPER_DEF
#define WASMVM_TEST_HELPER_DEF

#include <defines.h>
#include <WasmVM.h>
#include <hashmap_t.h>

byte_t* load_file(const char* filename, size_t* size);
int match_imports(const wasm_module module, const struct _hashmap* moduleInsts, size_t* size, ExternVal** externvals);

#endif