#ifndef WASMVM_DECODER_SECTION_DEF
#define WASMVM_DECODER_SECTION_DEF

#include <structures/WasmModule.h>
#include <dataTypes/FuncType.h>
#include <dataTypes/Value.h>
#include <dataTypes/vector_p.h>
#include <Loader.h>
#include <Executor.h>
#include <core/Store.h>

#define WASM_MAGIC      0x6D736100  // little endian
#define WASM_VERSION    0x01

#define TYPE_i32 0x7F
#define TYPE_i64 0x7E
#define TYPE_f32 0x7D
#define TYPE_f64 0x7C
#define TYPE_Func 0x60
#define TYPE_Table_anyfunc 0x70

#define IMPORT_Func 0x00
#define IMPORT_Table 0x01
#define IMPORT_Mem 0x02
#define IMPORT_Global 0x03

int parse_magic_version(WasmModule *module, uint8_t **read_p);
int parse_type_section(WasmModule *newModule, uint8_t **read_p, const uint8_t *end_p);
int parse_import_section(WasmModule *newModule, uint8_t **read_p, const uint8_t *end_p, Loader loader, Executor executor);
int parse_func_section(WasmModule *newModule, uint8_t **read_p, const uint8_t *end_p);
int parse_table_section(WasmModule *newModule, uint8_t **read_p, const uint8_t *end_p);
int parse_memory_section(WasmModule *newModule, uint8_t **read_p, const uint8_t *end_p);
int parse_global_section(WasmModule *newModule, uint8_t **read_p, const uint8_t *end_p);
int parse_export_section(WasmModule *newModule, uint8_t **read_p, const uint8_t *end_p);
int parse_start_section(WasmModule *newModule, uint8_t **read_p, const uint8_t *end_p);
int parse_element_section(WasmModule *newModule, uint8_t **read_p, const uint8_t *end_p);
int parse_code_section(WasmModule *newModule, uint8_t **read_p, const uint8_t *end_p);
int parse_data_section(WasmModule *newModule, uint8_t **read_p, const uint8_t *end_p);

#endif