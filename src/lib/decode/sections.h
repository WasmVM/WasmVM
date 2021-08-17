#ifndef WASMVM_DECODE_SECTIONS_DEF
#define WASMVM_DECODE_SECTIONS_DEF

#include <structures/WasmModule.h>

#define WASM_MAGIC      0x6D736100  // little endian
#define WASM_VERSION    0x01

#define TYPE_i32 0x7F
#define TYPE_i64 0x7E
#define TYPE_f32 0x7D
#define TYPE_f64 0x7C
#define TYPE_Func 0x60

#define IMPORT_Func 0x00
#define IMPORT_Table 0x01
#define IMPORT_Mem 0x02
#define IMPORT_Global 0x03

#define REF_funcref 0x70
#define REF_externref 0x6F

int skip_custom_section(const byte_t **read_p, const byte_t * const end_p);
int parse_magic_version(const byte_t **read_p, const byte_t * const end_p);
int parse_type_section(WasmModule *module, const byte_t **read_p, const byte_t * const end_p);
int parse_import_section(WasmModule *module, const byte_t **read_p, const byte_t *end_p);
int parse_func_section(WasmModule *module, const byte_t **read_p, const byte_t *end_p);
int parse_table_section(WasmModule *module, const byte_t **read_p, const byte_t *end_p);
int parse_memory_section(WasmModule *module, const byte_t **read_p, const byte_t *end_p);
int parse_global_section(WasmModule *module, const byte_t **read_p, const byte_t *end_p);
int parse_export_section(WasmModule *module, const byte_t **read_p, const byte_t *end_p);
int parse_start_section(WasmModule *module, const byte_t **read_p, const byte_t *end_p);
int parse_element_section(WasmModule *module, const byte_t **read_p, const byte_t *end_p);
// int parse_code_section(WasmModule *newModule, uint8_t **read_p, const uint8_t *end_p);
// int parse_data_section(WasmModule *newModule, uint8_t **read_p, const uint8_t *end_p);

// TODO: name section

#endif