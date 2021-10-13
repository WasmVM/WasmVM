#ifndef WASMVM_DEF
#define WASMVM_DEF

#include <defines.h>
#include <error.h>
#include <structures/WasmModule.h>
#include <Store.h>

int module_decode(const byte_t* data, const size_t size, wasm_module* module);
void module_free(wasm_module module);
wasm_store store_init();
void store_free(wasm_store store); // TODO:

// TODO: module_instantiate()
// TODO: module_validate()
// TODO: module_imports()
// TODO: module_exports()
// TODO: instance_export()
// TODO: func_alloc()
// TODO: func_type()
// TODO: func_invoke()
// TODO: table_alloc()
// TODO: table_type()
// TODO: table_read()
// TODO: table_write()
// TODO: table_size()
// TODO: table_grow()
// TODO: mem_alloc()
// TODO: mem_type()
// TODO: mem_read()
// TODO: mem_write()
// TODO: mem_size()
// TODO: mem_grow()
// TODO: global_alloc()
// TODO: global_type()
// TODO: global_read()
// TODO: global_write()

#endif