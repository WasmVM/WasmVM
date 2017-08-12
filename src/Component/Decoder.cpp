#include <Decoder.h>

void Decoder::decode(Store &store, Stack &coreStack){
    std::vector<char> &funcBody = store.funcs.at(coreStack.curLabel->funcIdx)->code.body;
    char bincode = funcBody.at((coreStack.curLabel->instrOffset));
    coreStack.curLabel->instrOffset += 1;
    switch(bincode){
        case OP_CTRL_END:
            Instruction::ctrl_end(coreStack);
        break;
        default:
#ifdef NDEBUG
        {
            char codeChr[2];
            sprintf(codeChr, "%02x", bincode);
            throw Exception(std::string("Unknown code: 0x") + codeChr, coreStack);
        }  
#endif
        break;
    }
}