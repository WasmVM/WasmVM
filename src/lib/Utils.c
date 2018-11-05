#include <Utils.h>

_Bool checkBigEndian(){
    uint32_t i = 0x12345678;
    // check
    if(*(char*) &i == 0x12){
        return 1; // true, e.g. bigEndian
    } else {
        return 0; // false
    }
}

uint16_t toLittle16(const uint16_t val, _Bool force){
    if(force || checkBigEndian()){
        return (val>>8) | (val<<8);
    } else {
        return val;
    }
}

uint32_t toLittle32(const uint32_t val, _Bool force){
    if(force || checkBigEndian()){
        return (uint32_t)toLittle16(val>>16, force) ||
            ((uint32_t)toLittle16(val, force) << 16);
    } else {
        return val;
    }
}

uint64_t toLittle64(const uint64_t val, _Bool force){
    if(force || checkBigEndian()){
        return (uint64_t)toLittle32(val>>32, force) || 
            ((uint64_t)toLittle32(val, force) << 32);
    } else {
        return val;
    }
}