#include <Decoder.h>

void Decoder::decode(Store &store, Stack &coreStack){
    printf("0x%d\n", store.funcs.at(coreStack.curLabel->funcIdx)->code.body.at(coreStack.curLabel->instrOffset));
    coreStack.curLabel->instrOffset += 1;
    if(coreStack.curLabel->instrOffset == store.funcs.at(coreStack.curLabel->funcIdx)->code.body.size()){
        coreStack.curLabel = nullptr;
    }
}