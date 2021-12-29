#ifndef WASMVM_TEST_HELPER_DEF
#define WASMVM_TEST_HELPER_DEF

#include <defines.h>
#include <WasmVM.h>
#include <hashmap_t.h>
#include <dataTypes/vector_t.h>

bytes_vector_t load_file(const char* filename);
externval_vector_t match_imports(const wasm_module module, const struct _hashmap* moduleInsts);

#endif