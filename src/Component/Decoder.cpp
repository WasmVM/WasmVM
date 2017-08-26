/*
Copyright (c) 2017 Luis Hsu

Licensed under the Apache License, Version 2.0 (the "License");you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and limitations under the License.
*/

#include <Decoder.h>

std::vector<char>* Decoder::funcBody = nullptr;
std::uint32_t* Decoder::curFuncIdx = nullptr;

void Decoder::decode(Store &store, Stack &coreStack){
    if(curFuncIdx == nullptr){
        curFuncIdx = new std::uint32_t;
        *curFuncIdx = coreStack.curLabel->funcIdx;
        funcBody = &(store.funcs.at(coreStack.curLabel->funcIdx)->code.body);
    }else if(coreStack.curLabel->funcIdx != *curFuncIdx){
        *curFuncIdx = coreStack.curLabel->funcIdx;
        funcBody = &(store.funcs.at(coreStack.curLabel->funcIdx)->code.body);
    }
    unsigned char bincode = funcBody->at(coreStack.curLabel->instrOffset);
    coreStack.curLabel->instrOffset += 1;
    switch(bincode){
        case OP_Ctrl_unreachable:
            Instruction::ctrl_unreachable(store, coreStack);
        break;
        case OP_Ctrl_nop:
        break;
        case OP_Ctrl_block:
            Instruction::ctrl_block(*funcBody, coreStack, funcBody->at(coreStack.curLabel->instrOffset++));
        break;
        case OP_Ctrl_loop:
            Instruction::ctrl_loop(coreStack, funcBody->at(coreStack.curLabel->instrOffset++));
        break;
        case OP_Ctrl_if:
            Instruction::ctrl_if(*funcBody, coreStack, funcBody->at(coreStack.curLabel->instrOffset++));
        break;
        case OP_Ctrl_else:
            Instruction::ctrl_else(coreStack);
        break;
        case OP_Ctrl_end:
            Instruction::ctrl_end(coreStack);
        break;
        case OP_Ctrl_br:
            try{
                Instruction::ctrl_br(Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset), coreStack);
            }catch(const char *e){
                throw Exception(std::string("[br] ") + e, coreStack);
            }
        break;
        case OP_Ctrl_br_if:
            try{
                Instruction::ctrl_br_if(Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset), coreStack);
            }catch(const char *e){
                throw Exception(std::string("[br_if] ") + e, coreStack);
            }
        break;
        case OP_Ctrl_br_table:
            try{
                std::uint32_t targetSize = Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset);
                std::vector<std::uint32_t> depths;
                while(targetSize-- > 0){
                    depths.push_back(Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset));
                }
                depths.push_back(Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset));
                Instruction::ctrl_br_table(depths, coreStack);
            }catch(const char *e){
                throw Exception(std::string("[br_table] ") + e, coreStack);
            }
        break;
        case OP_Ctrl_return:
            Instruction::ctrl_return(coreStack);
        break;
        case OP_Ctrl_call:
            try{
                Instruction::ctrl_call(Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset), store, coreStack);
            }catch(const char *e){
                throw Exception(std::string("[call] ") + e, coreStack);
            }
        break;
        case OP_Ctrl_call_indirect:
            try{
                Instruction::ctrl_call_indirect(Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset), store, coreStack);
            }catch(const char *e){
                throw Exception(std::string("[call_indirect] ") + e, coreStack);
            }
        break;
        case OP_i32_load:
            try{
                std::int32_t align = Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset);
                std::int32_t offset = Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset);
                Instruction::i32_load(align, offset, store, coreStack);
            }catch(const char *e){
                throw Exception(std::string("[i32.load] ") + e, coreStack);
            }
        break;
        case OP_i64_load:
            try{
                std::int32_t align = Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset);
                std::int32_t offset = Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset);
                Instruction::i64_load(align, offset, store, coreStack);
            }catch(const char *e){
                throw Exception(std::string("[i64.load] ") + e, coreStack);
            }
        break;
        case OP_f32_load:
            try{
                std::int32_t align = Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset);
                std::int32_t offset = Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset);
                Instruction::f32_load(align, offset, store, coreStack);
            }catch(const char *e){
                throw Exception(std::string("[f32.load] ") + e, coreStack);
            }
        break;
        case OP_f64_load:
            try{
                std::int32_t align = Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset);
                std::int32_t offset = Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset);
                Instruction::f64_load(align, offset, store, coreStack);
            }catch(const char *e){
                throw Exception(std::string("[f64.load] ") + e, coreStack);
            }
        break;
        case OP_i32_load8_s:
            try{
                std::int32_t align = Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset);
                std::int32_t offset = Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset);
                Instruction::i32_load8_s(align, offset, store, coreStack);
            }catch(const char *e){
                throw Exception(std::string("[i32.load8_s] ") + e, coreStack);
            }
        break;
        case OP_i32_load8_u:
            try{
                std::int32_t align = Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset);
                std::int32_t offset = Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset);
                Instruction::i32_load8_u(align, offset, store, coreStack);
            }catch(const char *e){
                throw Exception(std::string("[i32.load8_u] ") + e, coreStack);
            }
        break;
        case OP_i32_load16_s:
            try{
                std::int32_t align = Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset);
                std::int32_t offset = Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset);
                Instruction::i32_load16_s(align, offset, store, coreStack);
            }catch(const char *e){
                throw Exception(std::string("[i32.load16_s] ") + e, coreStack);
            }
        break;
        case OP_i32_load16_u:
            try{
                std::int32_t align = Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset);
                std::int32_t offset = Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset);
                Instruction::i32_load16_u(align, offset, store, coreStack);
            }catch(const char *e){
                throw Exception(std::string("[i32.load16_u] ") + e, coreStack);
            }
        break;
        case OP_i64_load8_s:
            try{
                std::int32_t align = Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset);
                std::int32_t offset = Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset);
                Instruction::i64_load8_s(align, offset, store, coreStack);
            }catch(const char *e){
                throw Exception(std::string("[i64.load8_s] ") + e, coreStack);
            }
        break;
        case OP_i64_load8_u:
            try{
                std::int32_t align = Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset);
                std::int32_t offset = Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset);
                Instruction::i64_load8_u(align, offset, store, coreStack);
            }catch(const char *e){
                throw Exception(std::string("[i64.load8_u] ") + e, coreStack);
            }
        break;
        case OP_i64_load16_s:
            try{
                std::int32_t align = Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset);
                std::int32_t offset = Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset);
                Instruction::i64_load16_s(align, offset, store, coreStack);
            }catch(const char *e){
                throw Exception(std::string("[i64.load16_s] ") + e, coreStack);
            }
        break;
        case OP_i64_load16_u:
            try{
                std::int32_t align = Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset);
                std::int32_t offset = Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset);
                Instruction::i64_load16_u(align, offset, store, coreStack);
            }catch(const char *e){
                throw Exception(std::string("[i64.load16_u] ") + e, coreStack);
            }
        break;
        case OP_i64_load32_s:
            try{
                std::int32_t align = Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset);
                std::int32_t offset = Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset);
                Instruction::i64_load32_s(align, offset, store, coreStack);
            }catch(const char *e){
                throw Exception(std::string("[i64.load32_s] ") + e, coreStack);
            }
        break;
        case OP_i64_load32_u:
            try{
                std::int32_t align = Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset);
                std::int32_t offset = Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset);
                Instruction::i64_load32_u(align, offset, store, coreStack);
            }catch(const char *e){
                throw Exception(std::string("[i64.load32_u] ") + e, coreStack);
            }
        break;
        case OP_i32_store:
            try{
                std::int32_t align = Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset);
                std::int32_t offset = Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset);
                Instruction::i32_store(align, offset, store, coreStack);
            }catch(const char *e){
                throw Exception(std::string("[i32.store] ") + e, coreStack);
            }
        break;
        case OP_i64_store:
            try{
                std::int32_t align = Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset);
                std::int32_t offset = Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset);
                Instruction::i64_store(align, offset, store, coreStack);
            }catch(const char *e){
                throw Exception(std::string("[i64.store] ") + e, coreStack);
            }
        break;
        case OP_f32_store:
            try{
                std::int32_t align = Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset);
                std::int32_t offset = Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset);
                Instruction::f32_store(align, offset, store, coreStack);
            }catch(const char *e){
                throw Exception(std::string("[f32.store] ") + e, coreStack);
            }
        break;
        case OP_f64_store:
            try{
                std::int32_t align = Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset);
                std::int32_t offset = Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset);
                Instruction::f64_store(align, offset, store, coreStack);
            }catch(const char *e){
                throw Exception(std::string("[f64.store] ") + e, coreStack);
            }
        break;
        case OP_i32_store8:
            try{
                std::int32_t align = Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset);
                std::int32_t offset = Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset);
                Instruction::i32_store8(align, offset, store, coreStack);
            }catch(const char *e){
                throw Exception(std::string("[i32.store8] ") + e, coreStack);
            }
        break;
        case OP_i32_store16:
            try{
                std::int32_t align = Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset);
                std::int32_t offset = Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset);
                Instruction::i32_store16(align, offset, store, coreStack);
            }catch(const char *e){
                throw Exception(std::string("[i32.store16] ") + e, coreStack);
            }
        break;
        case OP_i64_store8:
            try{
                std::int32_t align = Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset);
                std::int32_t offset = Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset);
                Instruction::i64_store8(align, offset, store, coreStack);
            }catch(const char *e){
                throw Exception(std::string("[i64.store8] ") + e, coreStack);
            }
        break;
        case OP_i64_store16:
            try{
                std::int32_t align = Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset);
                std::int32_t offset = Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset);
                Instruction::i64_store16(align, offset, store, coreStack);
            }catch(const char *e){
                throw Exception(std::string("[i64.store16] ") + e, coreStack);
            }
        break;
        case OP_i64_store32:
            try{
                std::int32_t align = Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset);
                std::int32_t offset = Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset);
                Instruction::i64_store32(align, offset, store, coreStack);
            }catch(const char *e){
                throw Exception(std::string("[i64.store32] ") + e, coreStack);
            }
        break;
        case OP_Current_memory:
            Instruction::current_memory(store, coreStack);
        break;
        case OP_Grow_memory:
            Instruction::grow_memory(store, coreStack);
        break;
        case OP_Drop:
            Instruction::drop(coreStack);
        break;
        case OP_Select:
            Instruction::select(coreStack);
        break;
        case OP_Get_local:
            try{
                Instruction::get_local(Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset), coreStack);
            }catch(const char *e){
                throw Exception(std::string("[get_local] ") + e, coreStack);
            }
        break;
        case OP_Set_local:
            try{
                Instruction::set_local(Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset), coreStack);
            }catch(const char *e){
                throw Exception(std::string("[set_local] ") + e, coreStack);
            }
        break;
        case OP_Tee_local:
            try{
                Instruction::tee_local(Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset), coreStack);
            }catch(const char *e){
                throw Exception(std::string("[tee_local] ") + e, coreStack);
            }
        break;
        case OP_Get_global:
            try{
                Instruction::get_global(Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset), store, coreStack);
            }catch(const char *e){
                throw Exception(std::string("[get_global] ") + e, coreStack);
            }
        break;
        case OP_Set_global:
            try{
                Instruction::set_global(Util::getLeb128_u32(*funcBody, coreStack.curLabel->instrOffset), store, coreStack);
            }catch(const char *e){
                throw Exception(std::string("[set_global] ") + e, coreStack);
            }
        break;
        case OP_i32_const:
            try{
                Instruction::i32_const(coreStack, Util::getLeb128_i32(*funcBody, coreStack.curLabel->instrOffset));
            }catch(const char *e){
                throw Exception(std::string("[i32.const] ") + e, coreStack);
            }
        break;
        case OP_i64_const:
            try{
                Instruction::i64_const(coreStack, Util::getLeb128_i64(*funcBody, coreStack.curLabel->instrOffset));
            }catch(const char *e){
                throw Exception(std::string("[i64.const] ") + e, coreStack);
            }
        break;
        case OP_f32_const:
            try{
                Instruction::f32_const(coreStack, Util::getIEEE754_f32(*funcBody, coreStack.curLabel->instrOffset));
            }catch(const char *e){
                throw Exception(std::string("[f32.const] ") + e, coreStack);
            }
        break;
        case OP_f64_const:
            try{
                Instruction::f64_const(coreStack, Util::getIEEE754_f64(*funcBody, coreStack.curLabel->instrOffset));
            }catch(const char *e){
                throw Exception(std::string("[f64.const] ") + e, coreStack);
            }
        break;
        case OP_i32_eqz:
            Instruction::i32_eqz(coreStack);
        break;
        case OP_i32_eq:
            Instruction::i32_eq(coreStack);
        break;
        case OP_i32_ne:
            Instruction::i32_ne(coreStack);
        break;
        case OP_i32_lt_s:
            Instruction::i32_lt_s(coreStack);
        break;
        case OP_i32_lt_u:
            Instruction::i32_lt_u(coreStack);
        break;
        case OP_i32_gt_s:
            Instruction::i32_gt_s(coreStack);
        break;
        case OP_i32_gt_u:
            Instruction::i32_gt_u(coreStack);
        break;
        case OP_i32_le_s:
            Instruction::i32_le_s(coreStack);
        break;
        case OP_i32_le_u:
            Instruction::i32_le_u(coreStack);
        break;
        case OP_i32_ge_s:
            Instruction::i32_ge_s(coreStack);
        break;
        case OP_i32_ge_u:
            Instruction::i32_ge_u(coreStack);
        break;
        case OP_i64_eqz:
            Instruction::i64_eqz(coreStack);
        break;
        case OP_i64_eq:
            Instruction::i64_eq(coreStack);
        break;
        case OP_i64_ne:
            Instruction::i64_ne(coreStack);
        break;
        case OP_i64_lt_s:
            Instruction::i64_lt_s(coreStack);
        break;
        case OP_i64_lt_u:
            Instruction::i64_lt_u(coreStack);
        break;
        case OP_i64_gt_s:
            Instruction::i64_gt_s(coreStack);
        break;
        case OP_i64_gt_u:
            Instruction::i64_gt_u(coreStack);
        break;
        case OP_i64_le_s:
            Instruction::i64_le_s(coreStack);
        break;
        case OP_i64_le_u:
            Instruction::i64_le_u(coreStack);
        break;
        case OP_i64_ge_s:
            Instruction::i64_ge_s(coreStack);
        break;
        case OP_i64_ge_u:
            Instruction::i64_ge_u(coreStack);
        break;
        case OP_f32_eq:
            Instruction::f32_eq(coreStack);
        break;
        case OP_f32_ne:
            Instruction::f32_ne(coreStack);
        break;
        case OP_f32_lt:
            Instruction::f32_lt(coreStack);
        break;
        case OP_f32_gt:
            Instruction::f32_gt(coreStack);
        break;
        case OP_f32_le:
            Instruction::f32_le(coreStack);
        break;
        case OP_f32_ge:
            Instruction::f32_ge(coreStack);
        break;
        case OP_f64_eq:
            Instruction::f64_eq(coreStack);
        break;
        case OP_f64_ne:
            Instruction::f64_ne(coreStack);
        break;
        case OP_f64_lt:
            Instruction::f64_lt(coreStack);
        break;
        case OP_f64_gt:
            Instruction::f64_gt(coreStack);
        break;
        case OP_f64_le:
            Instruction::f64_le(coreStack);
        break;
        case OP_f64_ge:
            Instruction::f64_ge(coreStack);
        break;
        case OP_i32_clz:
            Instruction::i32_clz(coreStack);
		break;
        case OP_i32_ctz:
            Instruction::i32_ctz(coreStack);
		break;
        case OP_i32_popcnt:
            Instruction::i32_popcnt(coreStack);
		break;
        case OP_i32_add:
            Instruction::i32_add(coreStack);
		break;
        case OP_i32_sub:
            Instruction::i32_sub(coreStack);
		break;
        case OP_i32_mul:
            Instruction::i32_mul(coreStack);
		break;
        case OP_i32_div_s:
            Instruction::i32_div_s(coreStack);
		break;
        case OP_i32_div_u:
            Instruction::i32_div_u(coreStack);
		break;
        case OP_i32_rem_s:
            Instruction::i32_rem_s(coreStack);
		break;
        case OP_i32_rem_u:
            Instruction::i32_rem_u(coreStack);
		break;
        case OP_i32_and:
            Instruction::i32_and(coreStack);
		break;
        case OP_i32_or:
            Instruction::i32_or(coreStack);
		break;
        case OP_i32_xor:
            Instruction::i32_xor(coreStack);
		break;
        case OP_i32_shl:
            Instruction::i32_shl(coreStack);
		break;
        case OP_i32_shr_s:
            Instruction::i32_shr_s(coreStack);
		break;
        case OP_i32_shr_u:
            Instruction::i32_shr_u(coreStack);
		break;
        case OP_i32_rotl:
            Instruction::i32_rotl(coreStack);
		break;
        case OP_i32_rotr:
            Instruction::i32_rotr(coreStack);
        break;
        case OP_i64_clz:
            Instruction::i64_clz(coreStack);
        break;
        case OP_i64_ctz:
            Instruction::i64_ctz(coreStack);
        break;
        case OP_i64_popcnt:
            Instruction::i64_popcnt(coreStack);
        break;
        case OP_i64_add:
            Instruction::i64_add(coreStack);
        break;
        case OP_i64_sub:
            Instruction::i64_sub(coreStack);
        break;
        case OP_i64_mul:
            Instruction::i64_mul(coreStack);
        break;
        case OP_i64_div_s:
            Instruction::i64_div_s(coreStack);
        break;
        case OP_i64_div_u:
            Instruction::i64_div_u(coreStack);
        break;
        case OP_i64_rem_s:
            Instruction::i64_rem_s(coreStack);
        break;
        case OP_i64_rem_u:
            Instruction::i64_rem_u(coreStack);
        break;
        case OP_i64_and:
            Instruction::i64_and(coreStack);
        break;
        case OP_i64_or:
            Instruction::i64_or(coreStack);
        break;
        case OP_i64_xor:
            Instruction::i64_xor(coreStack);
        break;
        case OP_i64_shl:
            Instruction::i64_shl(coreStack);
        break;
        case OP_i64_shr_s:
            Instruction::i64_shr_s(coreStack);
        break;
        case OP_i64_shr_u:
            Instruction::i64_shr_u(coreStack);
        break;
        case OP_i64_rotl:
            Instruction::i64_rotl(coreStack);
        break;
        case OP_i64_rotr:
            Instruction::i64_rotr(coreStack);
        break;
        case OP_f32_abs:
            Instruction::f32_abs(coreStack);
		break;
        case OP_f32_neg:
            Instruction::f32_neg(coreStack);
		break;
        case OP_f32_ceil:
            Instruction::f32_ceil(coreStack);
		break;
        case OP_f32_floor:
            Instruction::f32_floor(coreStack);
		break;
        case OP_f32_trunc:
            Instruction::f32_trunc(coreStack);
		break;
        case OP_f32_nearest:
            Instruction::f32_nearest(coreStack);
		break;
        case OP_f32_sqrt:
            Instruction::f32_sqrt(coreStack);
		break;
        case OP_f32_add:
            Instruction::f32_add(coreStack);
		break;
        case OP_f32_sub:
            Instruction::f32_sub(coreStack);
		break;
        case OP_f32_mul:
            Instruction::f32_mul(coreStack);
		break;
        case OP_f32_div:
            Instruction::f32_div(coreStack);
		break;
        case OP_f32_min:
            Instruction::f32_min(coreStack);
		break;
        case OP_f32_max:
            Instruction::f32_max(coreStack);
		break;
        case OP_f32_copysign:
            Instruction::f32_copysign(coreStack);
        break;
        case OP_f64_abs:
            Instruction::f64_abs(coreStack);
        break;
        case OP_f64_neg:
            Instruction::f64_neg(coreStack);
        break;
        case OP_f64_ceil:
            Instruction::f64_ceil(coreStack);
        break;
        case OP_f64_floor:
            Instruction::f64_floor(coreStack);
        break;
        case OP_f64_trunc:
            Instruction::f64_trunc(coreStack);
        break;
        case OP_f64_nearest:
            Instruction::f64_nearest(coreStack);
        break;
        case OP_f64_sqrt:
            Instruction::f64_sqrt(coreStack);
        break;
        case OP_f64_add:
            Instruction::f64_add(coreStack);
        break;
        case OP_f64_sub:
            Instruction::f64_sub(coreStack);
        break;
        case OP_f64_mul:
            Instruction::f64_mul(coreStack);
        break;
        case OP_f64_div:
            Instruction::f64_div(coreStack);
        break;
        case OP_f64_min:
            Instruction::f64_min(coreStack);
        break;
        case OP_f64_max:
            Instruction::f64_max(coreStack);
        break;
        case OP_f64_copysign:
            Instruction::f64_copysign(coreStack);
        break;
        case OP_i32_wrap_i64:
            Instruction::i32_wrap_i64(coreStack);
		break;
        case OP_i32_trunc_s_f32:
            Instruction::i32_trunc_s_f32(coreStack);
		break;
        case OP_i32_trunc_u_f32:
            Instruction::i32_trunc_u_f32(coreStack);
		break;
        case OP_i32_trunc_s_f64:
            Instruction::i32_trunc_s_f64(coreStack);
		break;
        case OP_i32_trunc_u_f64:
            Instruction::i32_trunc_u_f64(coreStack);
		break;
        case OP_i64_extend_s_i32:
            Instruction::i64_extend_s_i32(coreStack);
		break;
        case OP_i64_extend_u_i32:
            Instruction::i64_extend_u_i32(coreStack);
		break;
        case OP_i64_trunc_s_f32:
            Instruction::i64_trunc_s_f32(coreStack);
		break;
        case OP_i64_trunc_u_f32:
            Instruction::i64_trunc_u_f32(coreStack);
		break;
        case OP_i64_trunc_s_f64:
            Instruction::i64_trunc_s_f64(coreStack);
		break;
        case OP_i64_trunc_u_f64:
            Instruction::i64_trunc_u_f64(coreStack);
		break;
        case OP_f32_convert_s_i32:
            Instruction::f32_convert_s_i32(coreStack);
		break;
        case OP_f32_convert_u_i32:
            Instruction::f32_convert_u_i32(coreStack);
		break;
        case OP_f32_convert_s_i64:
            Instruction::f32_convert_s_i64(coreStack);
		break;
        case OP_f32_convert_u_i64:
            Instruction::f32_convert_u_i64(coreStack);
		break;
        case OP_f32_demote_f64:
            Instruction::f32_demote_f64(coreStack);
		break;
        case OP_f64_convert_s_i32:
            Instruction::f64_convert_s_i32(coreStack);
		break;
        case OP_f64_convert_u_i32:
            Instruction::f64_convert_u_i32(coreStack);
		break;
        case OP_f64_convert_s_i64:
            Instruction::f64_convert_s_i64(coreStack);
		break;
        case OP_f64_convert_u_i64:
            Instruction::f64_convert_u_i64(coreStack);
		break;
        case OP_f64_promote_f32:
            Instruction::f64_promote_f32(coreStack);
		break;
        case OP_i32_reinterpret_f32:
            Instruction::i32_reinterpret_f32(coreStack);
		break;
        case OP_i64_reinterpret_f64:
            Instruction::i64_reinterpret_f64(coreStack);
		break;
        case OP_f32_reinterpret_i32:
            Instruction::f32_reinterpret_i32(coreStack);
		break;
        case OP_f64_reinterpret_i64:
            Instruction::f64_reinterpret_i64(coreStack);
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
