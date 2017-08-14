#include <Decoder.h>

void Decoder::decode(Store &store, Stack &coreStack){
    std::vector<char> &funcBody = store.funcs.at(coreStack.curLabel->funcIdx)->code.body;
    char bincode = funcBody.at((coreStack.curLabel->instrOffset));
    coreStack.curLabel->instrOffset += 1;
    switch(bincode){
        case OP_CTRL_NOP:
        break;
        case OP_CTRL_BLOCK:
            Instruction::ctrl_block(coreStack, funcBody.at((coreStack.curLabel->instrOffset)));
        break;
        case OP_CTRL_LOOP:
            Instruction::ctrl_loop(coreStack, funcBody.at((coreStack.curLabel->instrOffset)));
        break;
        case OP_CTRL_END:
            Instruction::ctrl_end(coreStack);
        break;
        case OP_CTRL_BR:
            Instruction::ctrl_br(Util::get_uleb128_32(funcBody, coreStack.curLabel->instrOffset), coreStack);
            break;
        case OP_CTRL_RETURN:
            Instruction::ctrl_return(coreStack);
        break;
        case OP_CTRL_CALL:
            Instruction::ctrl_call(Util::get_uleb128_32(funcBody, coreStack.curLabel->instrOffset), store, coreStack);
        break;
        default:
        {
            char codeChr[2];
            sprintf(codeChr, "%02x", ((std::uint32_t)bincode) & 0xff);
#ifdef NDEBUG
            throw Exception(std::string("Unknown code: 0x") + codeChr, coreStack);
#else
            std::cout << Exception(std::string("Code: 0x") + codeChr, coreStack).desc << std::endl;
#endif
        }
        break;
    }
}