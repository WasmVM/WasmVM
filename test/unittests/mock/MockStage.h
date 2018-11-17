#ifndef WASMVM_TEST_UNITTESTS_MOCK_MOCKSTAGE_DEF
#define WASMVM_TEST_UNITTESTS_MOCK_MOCKSTAGE_DEF

#include <stdlib.h>
#include <Stage.h>

void mock_setInput(Stage* stage, void* input)
{
    stage->input = input;
}
void* mock_getOutput(Stage* stage)
{
    return stage->output;
}

Stage* new_MockStage(void* input, void* output, int (*run)(Stage*))
{
    Stage* newStage = (Stage*) malloc(sizeof(Stage));
    newStage->input = input;
    newStage->output = output;
    newStage->setInput = mock_setInput;
    newStage->getOutput = mock_getOutput;
    newStage->run = run;
    return newStage;
}

#endif