#include <Decoder.h>

void Decoder::decode(Store &store, Stack &coreStack){
    std::vector<char> &funcBody = store.funcs.at(coreStack.curLabel->funcIdx)->code.body;
    char bincode = funcBody.at((coreStack.curLabel->instrOffset));
    coreStack.curLabel->instrOffset += 1;
    switch(bincode){
        case OP_Ctrl_nop:
        break;
        case OP_Ctrl_block:
            Instruction::ctrl_block(funcBody, coreStack, funcBody.at(coreStack.curLabel->instrOffset++));
        break;
        case OP_Ctrl_loop:
            Instruction::ctrl_loop(coreStack, funcBody.at(coreStack.curLabel->instrOffset++));
        break;
        case OP_Ctrl_end:
            Instruction::ctrl_end(coreStack);
        break;
        case OP_Ctrl_br:
            Instruction::ctrl_br(Util::getLeb128_u32(funcBody, coreStack.curLabel->instrOffset), coreStack);
            break;
        case OP_Ctrl_return:
            Instruction::ctrl_return(coreStack);
        break;
        case OP_Ctrl_call:
            Instruction::ctrl_call(Util::getLeb128_u32(funcBody, coreStack.curLabel->instrOffset), store, coreStack);
        break;
        case OP_i32_const:
            Instruction::i32_const(coreStack, Util::getLeb128_i32(funcBody, coreStack.curLabel->instrOffset));
        break;
        case OP_i64_const:
            Instruction::i32_const(coreStack, Util::getLeb128_i64(funcBody, coreStack.curLabel->instrOffset));
        break;
        case OP_f32_const:
            Instruction::f32_const(coreStack, Util::getIEEE754_f32(funcBody, coreStack.curLabel->instrOffset));
        break;
        case OP_f64_const:
            Instruction::f64_const(coreStack, Util::getIEEE754_f64(funcBody, coreStack.curLabel->instrOffset));
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