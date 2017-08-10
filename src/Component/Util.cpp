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
        return (std::uint32_t)toLittle16(val >> 16, force) | ((std::uint32_t)toLittle16(val) << 16, force);
    }else{
        return val;
    }
}
std::uint64_t Util::toLittle64(const std::uint64_t &val, bool force){
    if(force || bigEndian){
        return (std::uint64_t)toLittle32(val >> 32, force) | ((std::uint64_t)toLittle32(val) << 32, force);
    }else{
        return val;
    }
}
std::uint32_t Util::get_uleb128_32(char* &ptr){
    std::uint32_t ret = 0;
    for(int i = 0; i < 5; ++i){
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
    return ret;
}
std::int32_t Util::get_leb128_32(char* &ptr){
    std::int32_t ret = 0;
    for(int i = 0; i < 5; ++i){
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
    return ret;
}
std::int64_t Util::get_leb128_64(char* &ptr){
    std::int64_t ret = 0;
    for(int i = 0; i < 10; ++i){
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
    return ret;
}