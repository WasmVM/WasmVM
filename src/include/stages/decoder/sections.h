#ifndef DECODER_SECTION
#define DECODER_SECTION

/*Section operations of Decoder.*/

#include <structures/WasmModule.h>
#include <dataTypes/FuncType.h>
#include <dataTypes/Value.h>

#define WASM_MAGIC      0x6D736100  // led
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


char skip_to_section(uint8_t sectionNum, uint8_t **ptr, uint8_t **endAddr);   // skip the current reading pointer to specified section

int parse_magic_version(WasmModule *module, uint8_t **read_p);
int parse_type_section(WasmModule *newModule, uint8_t **read_p, uint8_t **end_p);
int parse_import_section(WasmModule *newModule, uint32_t *importedFuncCount, uint8_t **read_p, uint8_t **end_p);

#endif