#ifndef WASMVM_DEF
#define WASMVM_DEF

#include <defines.h>
#include <error.h>
#include <structures/WasmModule.h>
#include <instance/ExternType.h>
#include <Store.h>

typedef vector_t(wasm_externtype) imports_vector_t;
typedef vector_t(byte_t) bytes_vector_t;

wasm_module module_decode(const bytes_vector_t bytes);
void module_free(wasm_module module);
wasm_store store_init();
void store_free(wasm_store store); // FIXME:
imports_vector_t module_imports(wasm_module module);
u32_t func_alloc(wasm_store store, wasm_functype functype, hostfunc_t hostfunc);
u32_t table_alloc(wasm_store store, wasm_table tabletype, wasm_ref ref);
// TODO: mem_alloc()
// TODO: global_alloc()

// TODO: int module_instantiate(wasm_store store, const wasm_module module, const size_t extern_size, const wasm_externval extern_val[extern_size], wasm_module_inst* module_inst);

// TODO: module_validate()
// TODO: module_exports()
// TODO: instance_export()
// TODO: func_type()
// TODO: func_invoke()
// TODO: table_type()
// TODO: table_read()
// TODO: table_write()
// TODO: table_size()
// TODO: table_grow()
// TODO: mem_type()
// TODO: mem_read()
// TODO: mem_write()
// TODO: mem_size()
// TODO: mem_grow()
// TODO: global_type()
// TODO: global_read()
// TODO: global_write()

#endif