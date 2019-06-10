#ifndef WASMVM_DATATYPES_LABEL_DEF
#define WASMVM_DATATYPES_LABEL_DEF

#include <stdint.h>
#include <dataTypes/vector_p.h>

typedef struct Label_ * Label;

Label new_Label(uint32_t funcAddr, uint32_t instrIndex, uint32_t contInstr);
void free_Label(Label label);

uint32_t label_get_funcAddr(Label label);
uint32_t label_get_instrIndex(Label label);
void label_set_instrIndex(Label label, uint32_t instrIndex);
uint32_t label_get_contInstr(Label label);
uint32_t label_get_endInstr(Label label);
void label_set_endInstr(Label label, uint32_t endInstr);
vector_p label_get_resultTypes(Label label);
void label_set_resultTypes(Label label, vector_p resultTypes);

#endif