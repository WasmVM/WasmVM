#include <instance/ControlInstrInst.h>

#include <stdlib.h>

void free_ControlInstrInst(ControlInstrInst* instance){
    free_vector(instance->indices);
    free(instance);
}