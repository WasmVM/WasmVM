#include <Util.h>

bool Util::bigEndian = true;

void Util::checkEndian(){
    // Check endian
    std::uint32_t i = 0x12345678;
    if(*(char *)&i == 0x12){
        bigEndian = true;
    }else{
        bigEndian = false;
    }
}
std::uint16_t Util::toLittle16(const std::uint16_t &val, bool force){
    if(force || bigEndian){
        return (val >> 8) | (val << 8);
    }else{
        return val;
    }
}
std::uint32_t Util::toLittle32(const std::uint32_t &val, bool force){
    if(force || bigEndian){
        return (std::uint32_t)toLittle16(val >> 16, force) | ((std::uint32_t)toLittle16(val, force) << 16);
    }else{
        return val;
    }
}
std::uint64_t Util::toLittle64(const std::uint64_t &val, bool force){
    if(force || bigEndian){
        return (std::uint64_t)toLittle32(val >> 32, force) | ((std::uint64_t)toLittle32(val, force) << 32);
    }else{
        return val;
    }
}
std::uint32_t Util::getLeb128_u32(char* &ptr, const char *max){
    std::uint32_t ret = 0;
    for(int i = 0; i < 5; ++i){
        if(ptr > max){
            throw "pointer exceed max while decode uleb128_32.";
        }
        std::uint32_t byte = *(ptr++);
        ret |= (byte & 0x7F) << (7 * i);
        // Check range
        if(i == 4){
            if(byte & 0x80){
                throw "Too much bytes while decode uleb128_32.";
            }
        }
        // Finished
        if((byte & 0x80) == 0){
            break;
        }
    }
    return toLittle32(ret);
}
std::int32_t Util::getLeb128_i32(char* &ptr, const char *max){
    std::int32_t ret = 0;
    for(int i = 0; i < 5; ++i){
        if(ptr > max){
            throw "pointer exceed max while decode leb128_32.";
        }
        std::int32_t byte = *(ptr++);
        ret |= (byte & 0x7F) << (7 * i);
        // Check range
        if(i == 4){
            if(byte & 0x80){
                throw "Too much bytes while decode leb128_32.";
            }
        }
        // Finished
        if((byte & 0x80) == 0){
            if(byte & 0x40){
                switch (i){
                case 0:
                    ret |= 0xFFFFFF80;
                    break;
                case 1:
                    ret |= 0xFFFFC000;
                    break;
                case 2:
                    ret |= 0xFFE00000;
                    break;
                case 3:
                    ret |= 0xF0000000;
                    break;
                default:
                    break;
                }
            }
            break;
        }
    }
    return toLittle32(ret);
}
std::int64_t Util::getLeb128_i64(char* &ptr, const char *max){
    std::int64_t ret = 0;
    for(int i = 0; i < 10; ++i){
        if(ptr > max){
            throw "pointer exceed max while decode leb128_64.";
        }
        std::int64_t byte = *(ptr++);
        ret |= (byte & 0x7F) << (7 * i);
        // Check range
        if(i == 9){
            if(byte & 0x80){
            throw "Too much bytes while decode leb128_64.";
            }
        }
        // Finished
        if((byte & 0x80) == 0){
            if(byte & 0x40){
                switch (i){
                    case 0:
                    ret |= 0xFFFFFFFFFFFFFF80;
                    break;
                    case 1:
                    ret |= 0xFFFFFFFFFFFFC000;
                    break;
                    case 2:
                    ret |= 0xFFFFFFFFFFE00000;
                    break;
                    case 3:
                    ret |= 0xFFFFFFFFF0000000;
                    break;
                    case 4:
                    ret |= 0xFFFFFFF800000000;
                    break;
                    case 5:
                    ret |= 0xFFFFFC0000000000;
                    break;
                    case 6:
                    ret |= 0xFFFE000000000000;
                    break;
                    case 7:
                    ret |= 0xFF00000000000000;
                    break;
                    case 8:
                    ret |= 0x8000000000000000;
                    break;
                    default:
                    break;
                }
            }
            break;
        }
    }
    return toLittle64(ret);
}
std::uint32_t Util::getLeb128_u32(std::vector<char> &funcBody, std::uint64_t &instrOffset){
    std::uint32_t ret = 0;
    for(int i = 0; i < 5; ++i){
        if(instrOffset >= funcBody.size()){
            throw "offset exceed function size while decode uleb128_32.";
        }
        std::uint32_t byte = funcBody.at(instrOffset++);
        ret |= (byte & 0x7F) << (7 * i);
        // Check range
        if(i == 4){
            if(byte & 0x80){
                throw "Too much bytes while decode uleb128_32.";
            }
        }
        // Finished
        if((byte & 0x80) == 0){
            break;
        }
    }
    return toLittle32(ret);
}
std::int32_t Util::getLeb128_i32(std::vector<char> &funcBody, std::uint64_t &instrOffset){
    std::int32_t ret = 0;
    for(int i = 0; i < 5; ++i){
        if(instrOffset >= funcBody.size()){
            throw "offset exceed function size while decode leb128_32.";
        }
        std::int32_t byte = funcBody.at(instrOffset++);
        ret |= (byte & 0x7F) << (7 * i);
        // Check range
        if(i == 4){
            if(byte & 0x80){
                throw "Too much bytes while decode leb128_32.";
            }
        }
        // Finished
        if((byte & 0x80) == 0){
            if(byte & 0x40){
                switch (i){
                case 0:
                    ret |= 0xFFFFFF80;
                    break;
                case 1:
                    ret |= 0xFFFFC000;
                    break;
                case 2:
                    ret |= 0xFFE00000;
                    break;
                case 3:
                    ret |= 0xF0000000;
                    break;
                default:
                    break;
                }
            }
            break;
        }
    }
    return toLittle32(ret);
}
std::int64_t Util::getLeb128_i64(std::vector<char> &funcBody, std::uint64_t &instrOffset){
    std::int64_t ret = 0;
    for(int i = 0; i < 10; ++i){
        if(instrOffset >= funcBody.size()){
            throw "offset exceed function size while decode leb128_64.";
        }
        std::int64_t byte = funcBody.at(instrOffset++);
        ret |= (byte & 0x7F) << (7 * i);
        // Check range
        if(i == 9){
            if(byte & 0x80){
            throw "Too much bytes while decode leb128_64.";
            }
        }
        // Finished
        if((byte & 0x80) == 0){
            if(byte & 0x40){
                switch (i){
                    case 0:
                    ret |= 0xFFFFFFFFFFFFFF80;
                    break;
                    case 1:
                    ret |= 0xFFFFFFFFFFFFC000;
                    break;
                    case 2:
                    ret |= 0xFFFFFFFFFFE00000;
                    break;
                    case 3:
                    ret |= 0xFFFFFFFFF0000000;
                    break;
                    case 4:
                    ret |= 0xFFFFFFF800000000;
                    break;
                    case 5:
                    ret |= 0xFFFFFC0000000000;
                    break;
                    case 6:
                    ret |= 0xFFFE000000000000;
                    break;
                    case 7:
                    ret |= 0xFF00000000000000;
                    break;
                    case 8:
                    ret |= 0x8000000000000000;
                    break;
                    default:
                    break;
                }
            }
            break;
        }
    }
    return toLittle64(ret);
}
float Util::getIEEE754_f32(std::vector<char> &funcBody, std::uint64_t &instrOffset){
    std::uint32_t bytes = 0;
    for(int i = 0; i < 4; ++i){
        if(instrOffset >= funcBody.size()){
            throw "offset exceed function size while decode IEEE754 float.";
        }
        bytes |= (((std::uint32_t)funcBody.at(instrOffset++)) & 0xff) << (i * 8);
    }
    bytes = toLittle32(bytes);
    return *((float *)&bytes);
}
double Util::getIEEE754_f64(std::vector<char> &funcBody, std::uint64_t &instrOffset){
    std::uint64_t bytes = 0;
    for(int i = 0; i < 8; ++i){
        if(instrOffset >= funcBody.size()){
            throw "offset exceed function size while decode IEEE754 float.";
        }
        bytes |= (((std::uint64_t)funcBody.at(instrOffset++)) & 0xff) << (i * 8);
    }
    bytes = toLittle64(bytes);
    return *((double *)&bytes);
}
std::uint64_t Util::getContinueOffset(std::vector<char> &funcBody, Stack &coreStack, std::uint64_t instrOffset, bool stopElse){
    unsigned int layer = 0;
    while(instrOffset < funcBody.size() - 1){
        switch(funcBody.at(instrOffset++)){
            // u32 immediate
            case OP_Ctrl_br:
            case OP_Ctrl_br_if:
            case OP_Ctrl_call:
            case OP_Ctrl_call_indirect:
            case OP_Get_local:
            case OP_Set_local:
            case OP_Tee_local:
            case OP_Get_global:
            case OP_Set_global:
                getLeb128_u32(funcBody, instrOffset);
            break;
            // memarg
            case OP_i32_load:
            case OP_i64_load:
            case OP_f32_load:
            case OP_f64_load:
            case OP_i32_load8_s:
            case OP_i32_load8_u:
            case OP_i32_load16_s:
            case OP_i32_load16_u:
            case OP_i64_load8_s:
            case OP_i64_load8_u:
            case OP_i64_load16_s:
            case OP_i64_load16_u:
            case OP_i64_load32_s:
            case OP_i64_load32_u:
            case OP_i32_store:
            case OP_i64_store:
            case OP_f32_store:
            case OP_f64_store:
            case OP_i32_store8:
            case OP_i32_store16:
            case OP_i64_store8:
            case OP_i64_store16:
            case OP_i64_store32:
                getLeb128_u32(funcBody, instrOffset); // align
                getLeb128_u32(funcBody, instrOffset); // offset
            break;
            // i32 immediate
            case OP_i32_const:
                getLeb128_i32(funcBody, instrOffset);
            break;
            // i64 immediate
            case OP_i64_const:
                getLeb128_i64(funcBody, instrOffset);
            break;
            // f32 immediate
            case OP_f32_const:
                instrOffset += 4;
            break;
            // f64 immediate
            case OP_f64_const:
                instrOffset += 8;
            break;
            // br_table
            case OP_Ctrl_br_table:
                for(std::uint32_t i = getLeb128_u32(funcBody, instrOffset); i >= 0; --i){
                    getLeb128_u32(funcBody, instrOffset);
                }
            break;
            // block
            case OP_Ctrl_block:
            case OP_Ctrl_loop:
            case OP_Ctrl_if:
                ++instrOffset; //blockType
                ++layer;
            break;
            // end
            case OP_Ctrl_end:
                if(layer != 0){
                    --layer;
                }else{
                    return --instrOffset;
                }
            break;
            // else
            case OP_Ctrl_else:
                if(layer == 0 && stopElse){
                    return instrOffset;
                }
            break;
            // others
            default:
            break;
        }
    }
    throw Exception("Can't found match end or else in this function.", coreStack);
}