#include <Validator.h>

#include <stdlib.h>
#include "Validates.h"

static int runValidator(Validator* validator)
{
    return validate_Module(validator->input);
}

Validator* new_Validator(WasmModule* module)
{
    Validator* validator = (Validator*) malloc(sizeof(Validator));
    validator->input = module;
    validator->output = module;
    validator->run = runValidator;
    return validator;
}