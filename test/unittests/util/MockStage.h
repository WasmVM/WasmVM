#ifndef WASMVM_TEST_UNITTESTS_UTIL_MOCKSTAGE_DEF
#define WASMVM_TEST_UNITTESTS_UTIL_MOCKSTAGE_DEF

#include <stdlib.h>
#include <Stage.h>

Stage* new_MockStage(void* input, void* output, int (*run)(Stage*))
{
    Stage* newStage = (Stage*) malloc(sizeof(Stage));
    newStage->input = input;
    newStage->output = output;
    newStage->run = run;
    return newStage;
}

#endif