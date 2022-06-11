#ifndef WASMVM_DEF
#define WASMVM_DEF

#include <defines.h>
#include <error.h>
#include <structures/WasmModule.h>
#include <instances/ExternType.h>
#include <Store.h>

typedef vector_t(wasm_externtype) imports_vector_t;
typedef vector_t(wasm_externval) externval_vector_t;
typedef vector_t(byte_t) bytes_vector_t;
typedef vector_t(wasm_value) values_vector_t;

wasm_module module_decode(const bytes_vector_t bytes);
void module_free(wasm_module module);
wasm_store store_init();
void store_free(wasm_store store);
imports_vector_t module_imports(wasm_module module);
u32_t func_alloc(wasm_store store, wasm_functype* const functype, hostfunc_t hostfunc);
u32_t table_alloc(wasm_store store, wasm_table tabletype, wasm_ref ref);
u32_t mem_alloc(wasm_store store, wasm_memory memtype);
u32_t global_alloc(wasm_store store, wasm_global global, wasm_value val);
wasm_module_inst module_instantiate(wasm_store store, const wasm_module module, externval_vector_t externval);

wasm_externval instance_export(wasm_module_inst module_inst, const unsigned name_size, const byte_t name[name_size]);
values_vector_t func_invoke(wasm_store store, u32_t funcaddr, values_vector_t args);

// TODO: module_validate()
// TODO: module_exports()
// TODO: func_type()
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