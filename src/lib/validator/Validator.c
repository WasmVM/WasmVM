#include <Validator.h>

#include <stdlib.h>

static void setInput(Validator* validator, void* input){
    validator->input = input;
}
static void* getOutput(Validator* validator){
    return validator->output;
}
static int runValidator(Validator* validator){
    return 0;
}

Validator* new_Validator(WasmModule* module){
    Validator* validator = (Validator*) malloc(sizeof(Validator));
    validator->input = module;
    validator->output = module;
    validator->setInput = setInput;
    validator->getOutput = getOutput;
    validator->run = runValidator;
    return validator;
}