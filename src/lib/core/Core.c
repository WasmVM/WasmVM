#include <core/Core.h>

#include <stdio.h>
#include <stdint.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <Opcodes.h>
#include <core/Runtime.h>
#include <dataTypes/stack.h>
#include <dataTypes/Value.h>
#include <dataTypes/Label.h>
#include <dataTypes/Frame_.h>
#include <dataTypes/Entry.h>
#include <instance/FuncInst.h>
#include <instance/InstrInst.h>
#include <instance/MemInst.h>
#include <instance/ControlInstrInst.h>
#include <instance/ParametricInstrInst.h>
#include <instance/VariableInstrInst.h>
#include <instance/MemoryInstrInst.h>
#include <instance/NumericInstrInst.h>

static int run_control_instr(Stack* stack, Store* store, ControlInstrInst* instr, uint8_t opcode)
{
    int result = 0;
    switch (opcode) {
        case Op_unreachable:
            // TODO:
            break;
        case Op_nop:
            result = runtime_nop();
            label_set_instrIndex(stack->curLabel, label_get_instrIndex(stack->curLabel) + 1);
            break;
        case Op_block:
            // TODO:
            break;
        case Op_loop:
            // TODO:
            break;
        case Op_if:
            return runtime_if(stack, instr);
        case Op_else:
            return runtime_else(stack);
        case Op_end:
            return runtime_end(stack, store);
            break;
        case Op_br:
            // TODO:
            break;
        case Op_br_if:
            // TODO:
            break;
        case Op_br_table:
            // TODO:
            break;
        case Op_return:
            // TODO:
            break;
        case Op_call:
            return runtime_call(stack, store, instr);
            break;
        case Op_call_indirect:
            // TODO:
            break;
        default:
            break;
    }
    return result;
}

static int run_parametric_instr(Stack* stack, uint8_t opcode)
{
    int result = 0;
    switch (opcode) {
        case Op_drop:
            result = runtime_drop(stack);
            break;
        case Op_select:
            result = runtime_select(stack);
            break;
        default:
            break;
    }
    label_set_instrIndex(stack->curLabel, label_get_instrIndex(stack->curLabel) + 1);
    return result;
}

static int run_variable_instr(Stack* stack, VariableInstrInst* instr, uint8_t opcode)
{
    int result = 0;
    switch (opcode) {
        case Op_get_local:
            // TODO:
            break;
        case Op_set_local:
            // TODO:
            break;
        case Op_tee_local:
            // TODO:
            break;
        case Op_get_global:
            // TODO:
            break;
        case Op_set_global:
            // TODO:
            break;
        default:
            break;
    }
    label_set_instrIndex(stack->curLabel, label_get_instrIndex(stack->curLabel) + 1);
    return result;
}

static int run_memory_instr(Stack* stack, Store* store, ModuleInst* module, MemoryInstrInst* instr, uint8_t opcode)
{
    MemInst* memory = (MemInst*)store->mems->at(store->mems, *(uint32_t*)module->memaddrs->at(module->memaddrs, 0));
    int result = 0;
    switch (opcode) {
        case Op_i32_load:
            result = runtime_i32_load(stack, memory, instr->offset, instr->align);
            break;
        case Op_i64_load:
            result = runtime_i64_load(stack, memory, instr->offset, instr->align);
            break;
        case Op_f32_load:
            result = runtime_f32_load(stack, memory, instr->offset, instr->align);
            break;
        case Op_f64_load:
            result = runtime_f64_load(stack, memory, instr->offset, instr->align);
            break;
        case Op_i32_load8_s:
            result = runtime_i32_load8_s(stack, memory, instr->offset, instr->align);
            break;
        case Op_i32_load8_u:
            result = runtime_i32_load8_u(stack, memory, instr->offset, instr->align);
            break;
        case Op_i32_load16_s:
            result = runtime_i32_load16_s(stack, memory, instr->offset, instr->align);
            break;
        case Op_i32_load16_u:
            result = runtime_i32_load16_u(stack, memory, instr->offset, instr->align);
            break;
        case Op_i64_load8_s:
            result = runtime_i64_load8_s(stack, memory, instr->offset, instr->align);
            break;
        case Op_i64_load8_u:
            result = runtime_i64_load8_u(stack, memory, instr->offset, instr->align);
            break;
        case Op_i64_load16_s:
            result = runtime_i64_load16_s(stack, memory, instr->offset, instr->align);
            break;
        case Op_i64_load16_u:
            result = runtime_i64_load16_u(stack, memory, instr->offset, instr->align);
            break;
        case Op_i64_load32_s:
            result = runtime_i64_load32_s(stack, memory, instr->offset, instr->align);
            break;
        case Op_i64_load32_u:
            result = runtime_i64_load32_u(stack, memory, instr->offset, instr->align);
            break;
        case Op_i32_store:
            result = runtime_i32_store(stack, memory, instr->offset, instr->align);
            break;
        case Op_i64_store:
            result = runtime_i64_store(stack, memory, instr->offset, instr->align);
            break;
        case Op_f32_store:
            result = runtime_f32_store(stack, memory, instr->offset, instr->align);
            break;
        case Op_f64_store:
            result = runtime_f64_store(stack, memory, instr->offset, instr->align);
            break;
        case Op_i32_store8:
            result = runtime_i32_store8(stack, memory, instr->offset, instr->align);
            break;
        case Op_i32_store16:
            result = runtime_i32_store16(stack, memory, instr->offset, instr->align);
            break;
        case Op_i64_store8:
            result = runtime_i64_store8(stack, memory, instr->offset, instr->align);
            break;
        case Op_i64_store16:
            result = runtime_i64_store16(stack, memory, instr->offset, instr->align);
            break;
        case Op_i64_store32:
            result = runtime_i64_store32(stack, memory, instr->offset, instr->align);
            break;
        case Op_memory_size:
            // TODO:
            break;
        case Op_memory_grow:
            // TODO:
            break;
        default:
            break;
    }
    label_set_instrIndex(stack->curLabel, label_get_instrIndex(stack->curLabel) + 1);
    return result;
}

static int run_numeric_instr(Stack* stack, NumericInstrInst* instr, uint8_t opcode)
{
    int result = 0;
    switch (opcode) {
        case Op_i32_const:
            result = runtime_i32_const(stack, instr->constant.value.i32);
            break;
        case Op_i64_const:
            result = runtime_i64_const(stack, instr->constant.value.i64);
            break;
        case Op_f32_const:
            result = runtime_f32_const(stack, instr->constant.value.f32);
            break;
        case Op_f64_const:
            result = runtime_f64_const(stack, instr->constant.value.f64);
            break;
        case Op_i32_eqz:
            result = runtime_i32_eqz(stack);
            break;
        case Op_i32_eq:
            result = runtime_i32_eq(stack);
            break;
        case Op_i32_ne:
            result = runtime_i32_ne(stack);
            break;
        case Op_i32_lt_s:
            result = runtime_i32_lt_s(stack);
            break;
        case Op_i32_lt_u:
            result = runtime_i32_lt_u(stack);
            break;
        case Op_i32_gt_s:
            result = runtime_i32_gt_s(stack);
            break;
        case Op_i32_gt_u:
            result = runtime_i32_gt_u(stack);
            break;
        case Op_i32_le_s:
            result = runtime_i32_le_s(stack);
            break;
        case Op_i32_le_u:
            result = runtime_i32_le_u(stack);
            break;
        case Op_i32_ge_s:
            result = runtime_i32_ge_s(stack);
            break;
        case Op_i32_ge_u:
            result = runtime_i32_ge_u(stack);
            break;
        case Op_i64_eqz:
            result = runtime_i64_eqz(stack);
            break;
        case Op_i64_eq:
            result = runtime_i64_eq(stack);
            break;
        case Op_i64_ne:
            result = runtime_i64_ne(stack);
            break;
        case Op_i64_lt_s:
            result = runtime_i64_lt_s(stack);
            break;
        case Op_i64_lt_u:
            result = runtime_i64_lt_u(stack);
            break;
        case Op_i64_gt_s:
            result = runtime_i64_gt_s(stack);
            break;
        case Op_i64_gt_u:
            result = runtime_i64_gt_u(stack);
            break;
        case Op_i64_le_s:
            result = runtime_i64_le_s(stack);
            break;
        case Op_i64_le_u:
            result = runtime_i64_le_u(stack);
            break;
        case Op_i64_ge_s:
            result = runtime_i64_ge_s(stack);
            break;
        case Op_i64_ge_u:
            result = runtime_i64_ge_u(stack);
            break;
        case Op_f32_eq:
            result = runtime_f32_eq(stack);
            break;
        case Op_f32_ne:
            result = runtime_f32_ne(stack);
            break;
        case Op_f32_lt:
            result = runtime_f32_lt(stack);
            break;
        case Op_f32_gt:
            result = runtime_f32_gt(stack);
            break;
        case Op_f32_le:
            result = runtime_f32_le(stack);
            break;
        case Op_f32_ge:
            result = runtime_f32_ge(stack);
            break;
        case Op_f64_eq:
            result = runtime_f64_eq(stack);
            break;
        case Op_f64_ne:
            result = runtime_f64_ne(stack);
            break;
        case Op_f64_lt:
            result = runtime_f64_lt(stack);
            break;
        case Op_f64_gt:
            result = runtime_f64_gt(stack);
            break;
        case Op_f64_le:
            result = runtime_f64_le(stack);
            break;
        case Op_f64_ge:
            result = runtime_f64_ge(stack);
            break;
        case Op_i32_clz:
            result = runtime_i32_clz(stack);
            break;
        case Op_i32_ctz:
            result = runtime_i32_ctz(stack);
            break;
        case Op_i32_popcnt:
            result = runtime_i32_popcnt(stack);
            break;
        case Op_i32_add:
            result = runtime_i32_add(stack);
            break;
        case Op_i32_sub:
            result = runtime_i32_sub(stack);
            break;
        case Op_i32_mul:
            result = runtime_i32_mul(stack);
            break;
        case Op_i32_div_s:
            result = runtime_i32_div_s(stack);
            break;
        case Op_i32_div_u:
            result = runtime_i32_div_u(stack);
            break;
        case Op_i32_rem_s:
            result = runtime_i32_rem_s(stack);
            break;
        case Op_i32_rem_u:
            result = runtime_i32_rem_u(stack);
            break;
        case Op_i32_and:
            result = runtime_i32_and(stack);
            break;
        case Op_i32_or:
            result = runtime_i32_or(stack);
            break;
        case Op_i32_xor:
            result = runtime_i32_xor(stack);
            break;
        case Op_i32_shl:
            result = runtime_i32_shl(stack);
            break;
        case Op_i32_shr_s:
            result = runtime_i32_shr_s(stack);
            break;
        case Op_i32_shr_u:
            result = runtime_i32_shr_u(stack);
            break;
        case Op_i32_rotl:
            result = runtime_i32_rotl(stack);
            break;
        case Op_i32_rotr:
            result = runtime_i32_rotr(stack);
            break;
        case Op_i64_clz:
            result = runtime_i64_clz(stack);
            break;
        case Op_i64_ctz:
            result = runtime_i64_ctz(stack);
            break;
        case Op_i64_popcnt:
            result = runtime_i64_popcnt(stack);
            break;
        case Op_i64_add:
            result = runtime_i64_add(stack);
            break;
        case Op_i64_sub:
            result = runtime_i64_sub(stack);
            break;
        case Op_i64_mul:
            result = runtime_i64_mul(stack);
            break;
        case Op_i64_div_s:
            result = runtime_i64_div_s(stack);
            break;
        case Op_i64_div_u:
            result = runtime_i64_div_u(stack);
            break;
        case Op_i64_rem_s:
            result = runtime_i64_rem_s(stack);
            break;
        case Op_i64_rem_u:
            result = runtime_i64_rem_u(stack);
            break;
        case Op_i64_and:
            result = runtime_i64_and(stack);
            break;
        case Op_i64_or:
            result = runtime_i64_or(stack);
            break;
        case Op_i64_xor:
            result = runtime_i64_xor(stack);
            break;
        case Op_i64_shl:
            result = runtime_i64_shl(stack);
            break;
        case Op_i64_shr_s:
            result = runtime_i64_shr_s(stack);
            break;
        case Op_i64_shr_u:
            result = runtime_i64_shr_u(stack);
            break;
        case Op_i64_rotl:
            result = runtime_i64_rotl(stack);
            break;
        case Op_i64_rotr:
            result = runtime_i64_rotr(stack);
            break;
        case Op_f32_abs:
            result = runtime_f32_abs(stack);
            break;
        case Op_f32_neg:
            result = runtime_f32_neg(stack);
            break;
        case Op_f32_ceil:
            result = runtime_f32_ceil(stack);
            break;
        case Op_f32_floor:
            result = runtime_f32_floor(stack);
            break;
        case Op_f32_trunc:
            result = runtime_f32_trunc(stack);
            break;
        case Op_f32_nearest:
            result = runtime_f32_nearest(stack);
            break;
        case Op_f32_sqrt:
            result = runtime_f32_sqrt(stack);
            break;
        case Op_f32_add:
            result = runtime_f32_add(stack);
            break;
        case Op_f32_sub:
            result = runtime_f32_sub(stack);
            break;
        case Op_f32_mul:
            result = runtime_f32_mul(stack);
            break;
        case Op_f32_div:
            result = runtime_f32_div(stack);
            break;
        case Op_f32_min:
            result = runtime_f32_min(stack);
            break;
        case Op_f32_max:
            result = runtime_f32_max(stack);
            break;
        case Op_f32_copysign:
            result = runtime_f32_copysign(stack);
            break;
        case Op_f64_abs:
            result = runtime_f64_abs(stack);
            break;
        case Op_f64_neg:
            result = runtime_f64_neg(stack);
            break;
        case Op_f64_ceil:
            result = runtime_f64_ceil(stack);
            break;
        case Op_f64_floor:
            result = runtime_f64_floor(stack);
            break;
        case Op_f64_trunc:
            result = runtime_f64_trunc(stack);
            break;
        case Op_f64_nearest:
            result = runtime_f64_nearest(stack);
            break;
        case Op_f64_sqrt:
            result = runtime_f64_sqrt(stack);
            break;
        case Op_f64_add:
            result = runtime_f64_add(stack);
            break;
        case Op_f64_sub:
            result = runtime_f64_sub(stack);
            break;
        case Op_f64_mul:
            result = runtime_f64_mul(stack);
            break;
        case Op_f64_div:
            result = runtime_f64_div(stack);
            break;
        case Op_f64_min:
            result = runtime_f64_min(stack);
            break;
        case Op_f64_max:
            result = runtime_f64_max(stack);
            break;
        case Op_f64_copysign:
            result = runtime_f64_copysign(stack);
            break;
        case Op_i32_wrap_i64:
            result = runtime_i32_wrap_i64(stack);
            break;
        case Op_i32_trunc_s_f32:
            result = runtime_i32_trunc_s_f32(stack);
            break;
        case Op_i32_trunc_u_f32:
            result = runtime_i32_trunc_u_f32(stack);
            break;
        case Op_i32_trunc_s_f64:
            result = runtime_i32_trunc_s_f64(stack);
            break;
        case Op_i32_trunc_u_f64:
            result = runtime_i32_trunc_u_f64(stack);
            break;
        case Op_i64_extend_s_i32:
            result = runtime_i64_extend_s_i32(stack);
            break;
        case Op_i64_extend_u_i32:
            result = runtime_i64_extend_u_i32(stack);
            break;
        case Op_i64_trunc_s_f32:
            result = runtime_i64_trunc_s_f32(stack);
            break;
        case Op_i64_trunc_u_f32:
            result = runtime_i64_trunc_u_f32(stack);
            break;
        case Op_i64_trunc_s_f64:
            result = runtime_i64_trunc_s_f64(stack);
            break;
        case Op_i64_trunc_u_f64:
            result = runtime_i64_trunc_u_f64(stack);
            break;
        case Op_f32_convert_s_i32:
            result = runtime_f32_convert_s_i32(stack);
            break;
        case Op_f32_convert_u_i32:
            result = runtime_f32_convert_u_i32(stack);
            break;
        case Op_f32_convert_s_i64:
            result = runtime_f32_convert_s_i64(stack);
            break;
        case Op_f32_convert_u_i64:
            result = runtime_f32_convert_u_i64(stack);
            break;
        case Op_f32_demote_f64:
            result = runtime_f32_demote_f64(stack);
            break;
        case Op_f64_convert_s_i32:
            result = runtime_f64_convert_s_i32(stack);
            break;
        case Op_f64_convert_u_i32:
            result = runtime_f64_convert_u_i32(stack);
            break;
        case Op_f64_convert_s_i64:
            result = runtime_f64_convert_s_i64(stack);
            break;
        case Op_f64_convert_u_i64:
            result = runtime_f64_convert_u_i64(stack);
            break;
        case Op_f64_promote_f32:
            result = runtime_f64_promote_f32(stack);
            break;
        case Op_i32_reinterpret_f32:
            result = runtime_i32_reinterpret_f32(stack);
            break;
        case Op_i64_reinterpret_f64:
            result = runtime_i64_reinterpret_f64(stack);
            break;
        case Op_f32_reinterpret_i32:
            result = runtime_f32_reinterpret_i32(stack);
            break;
        case Op_f64_reinterpret_i64:
            result = runtime_f64_reinterpret_i64(stack);
            break;
        default:
            break;
    }
    label_set_instrIndex(stack->curLabel, label_get_instrIndex(stack->curLabel) + 1);
    return result;
}

static void* exec_Core(void* corePtr)
{
    Core* core = (Core*) corePtr;
    int* result = (int*) malloc(sizeof(int));
    *result = 0;
    while (core->status == Core_Running && *result == 0 && core->stack->curFrame) {
        FuncInst* func = (FuncInst*) core->executor->store->funcs->at(core->executor->store->funcs, label_get_funcAddr(core->stack->curLabel));
        if(label_get_instrIndex(core->stack->curLabel) >= func->code->size) {
            Label label = NULL;
            if(pop_Label(core->stack, &label)) {
                core->status = Core_Stop;
                *result = -1;
                pthread_exit(result);
            }

            stack* valStack = new_stack(NULL);
            for(uint32_t i = 0; i < func->type->results->length; ++i) {
                Value* retValue = NULL;
                pop_Value(core->stack, &retValue);
                valStack->push(valStack, retValue);
            }

            Frame frame = NULL;
            pop_Frame(core->stack, &frame);
            for(uint32_t i = 0; i < func->type->results->length; ++i) {
                ValueType* resultType = (ValueType*)func->type->results->at(func->type->results, i);
                Value* retValue = NULL;
                valStack->pop(valStack, (void**)&retValue);
                push_Value(core->stack, retValue);
            }
            free_stack(valStack);
            free_Label(label);
            free_Frame(frame);
            continue;
        }
        InstrInst* instr = (InstrInst*)func->code->at(func->code, label_get_instrIndex(core->stack->curLabel));
        switch (instr->opcode) {
            case Op_unreachable:
            case Op_nop:
            case Op_block:
            case Op_loop:
            case Op_if:
            case Op_else:
            case Op_end:
            case Op_br:
            case Op_br_if:
            case Op_br_table:
            case Op_return:
            case Op_call:
            case Op_call_indirect:
                *result = run_control_instr(core->stack, core->executor->store, (ControlInstrInst*)instr, instr->opcode);
                break;
            case Op_drop:
            case Op_select:
                *result = run_parametric_instr(core->stack, instr->opcode);
                break;
            case Op_get_local:
            case Op_set_local:
            case Op_tee_local:
            case Op_get_global:
            case Op_set_global:
                *result = run_variable_instr(core->stack, (VariableInstrInst*)instr, instr->opcode);
                break;
            case Op_i32_load:
            case Op_i64_load:
            case Op_f32_load:
            case Op_f64_load:
            case Op_i32_load8_s:
            case Op_i32_load8_u:
            case Op_i32_load16_s:
            case Op_i32_load16_u:
            case Op_i64_load8_s:
            case Op_i64_load8_u:
            case Op_i64_load16_s:
            case Op_i64_load16_u:
            case Op_i64_load32_s:
            case Op_i64_load32_u:
            case Op_i32_store:
            case Op_i64_store:
            case Op_f32_store:
            case Op_f64_store:
            case Op_i32_store8:
            case Op_i32_store16:
            case Op_i64_store8:
            case Op_i64_store16:
            case Op_i64_store32:
            case Op_memory_size:
            case Op_memory_grow:
                *result = run_memory_instr(core->stack, core->executor->store, core->module, (MemoryInstrInst*)instr, instr->opcode);
                break;
            case Op_i32_const:
            case Op_i64_const:
            case Op_f32_const:
            case Op_f64_const:
            case Op_i32_eqz:
            case Op_i32_eq:
            case Op_i32_ne:
            case Op_i32_lt_s:
            case Op_i32_lt_u:
            case Op_i32_gt_s:
            case Op_i32_gt_u:
            case Op_i32_le_s:
            case Op_i32_le_u:
            case Op_i32_ge_s:
            case Op_i32_ge_u:
            case Op_i64_eqz:
            case Op_i64_eq:
            case Op_i64_ne:
            case Op_i64_lt_s:
            case Op_i64_lt_u:
            case Op_i64_gt_s:
            case Op_i64_gt_u:
            case Op_i64_le_s:
            case Op_i64_le_u:
            case Op_i64_ge_s:
            case Op_i64_ge_u:
            case Op_f32_eq:
            case Op_f32_ne:
            case Op_f32_lt:
            case Op_f32_gt:
            case Op_f32_le:
            case Op_f32_ge:
            case Op_f64_eq:
            case Op_f64_ne:
            case Op_f64_lt:
            case Op_f64_gt:
            case Op_f64_le:
            case Op_f64_ge:
            case Op_i32_clz:
            case Op_i32_ctz:
            case Op_i32_popcnt:
            case Op_i32_add:
            case Op_i32_sub:
            case Op_i32_mul:
            case Op_i32_div_s:
            case Op_i32_div_u:
            case Op_i32_rem_s:
            case Op_i32_rem_u:
            case Op_i32_and:
            case Op_i32_or:
            case Op_i32_xor:
            case Op_i32_shl:
            case Op_i32_shr_s:
            case Op_i32_shr_u:
            case Op_i32_rotl:
            case Op_i32_rotr:
            case Op_i64_clz:
            case Op_i64_ctz:
            case Op_i64_popcnt:
            case Op_i64_add:
            case Op_i64_sub:
            case Op_i64_mul:
            case Op_i64_div_s:
            case Op_i64_div_u:
            case Op_i64_rem_s:
            case Op_i64_rem_u:
            case Op_i64_and:
            case Op_i64_or:
            case Op_i64_xor:
            case Op_i64_shl:
            case Op_i64_shr_s:
            case Op_i64_shr_u:
            case Op_i64_rotl:
            case Op_i64_rotr:
            case Op_f32_abs:
            case Op_f32_neg:
            case Op_f32_ceil:
            case Op_f32_floor:
            case Op_f32_trunc:
            case Op_f32_nearest:
            case Op_f32_sqrt:
            case Op_f32_add:
            case Op_f32_sub:
            case Op_f32_mul:
            case Op_f32_div:
            case Op_f32_min:
            case Op_f32_max:
            case Op_f32_copysign:
            case Op_f64_abs:
            case Op_f64_neg:
            case Op_f64_ceil:
            case Op_f64_floor:
            case Op_f64_trunc:
            case Op_f64_nearest:
            case Op_f64_sqrt:
            case Op_f64_add:
            case Op_f64_sub:
            case Op_f64_mul:
            case Op_f64_div:
            case Op_f64_min:
            case Op_f64_max:
            case Op_f64_copysign:
            case Op_i32_wrap_i64:
            case Op_i32_trunc_s_f32:
            case Op_i32_trunc_u_f32:
            case Op_i32_trunc_s_f64:
            case Op_i32_trunc_u_f64:
            case Op_i64_extend_s_i32:
            case Op_i64_extend_u_i32:
            case Op_i64_trunc_s_f32:
            case Op_i64_trunc_u_f32:
            case Op_i64_trunc_s_f64:
            case Op_i64_trunc_u_f64:
            case Op_f32_convert_s_i32:
            case Op_f32_convert_u_i32:
            case Op_f32_convert_s_i64:
            case Op_f32_convert_u_i64:
            case Op_f32_demote_f64:
            case Op_f64_convert_s_i32:
            case Op_f64_convert_u_i32:
            case Op_f64_convert_s_i64:
            case Op_f64_convert_u_i64:
            case Op_f64_promote_f32:
            case Op_i32_reinterpret_f32:
            case Op_i64_reinterpret_f64:
            case Op_f32_reinterpret_i32:
            case Op_f64_reinterpret_i64:
                *result = run_numeric_instr(core->stack, (NumericInstrInst*)instr, instr->opcode);
                break;
            default:
                break;
        }
    }
    if(core->status == Core_Running) {
        core->status = Core_Stop;
    }
    atomic_fetch_sub(&(core->executor->runningCores), 1);
    pthread_cond_signal(&(core->executor->cond));
    pthread_exit(result);
}

static int run_Core(Core* core)
{
    if(core->status != Core_Stop) {
        return -1;
    }
    core->status = Core_Running;
    atomic_fetch_add(&(core->executor->runningCores), 1);
    core->stack = new_Stack();
    // Get function instance
    FuncInst* startFunc = (FuncInst*)core->executor->store->funcs->at(core->executor->store->funcs, core->startFuncAddr);
    // Set frame
    Frame frame = new_Frame(startFunc->module);
    // Set local values of start function
    for(uint32_t i = 0; i < startFunc->locals->length; ++i) {
        switch (*(ValueType*)startFunc->locals->at(startFunc->locals, i)) {
            case Value_i32:
                frame->locals->push_back(frame->locals, new_i32Value(0));
                break;
            case Value_i64:
                frame->locals->push_back(frame->locals, new_i64Value(0));
                break;
            case Value_f32:
                frame->locals->push_back(frame->locals, new_f32Value(0));
                break;
            case Value_f64:
                frame->locals->push_back(frame->locals, new_f64Value(0));
                break;
            default:
                break;
        }
    }
    push_Frame(core->stack, frame);
    Label label = new_Label(core->startFuncAddr, 0, startFunc->code->size);
    label_set_resultTypes(label, startFunc->type->results);
    push_Label(core->stack, label);
    // Run in thread
    return pthread_create(&core->thread, NULL, exec_Core, (void*)core);
}

static int stop_core(Core* core)
{
    core->status = Core_Stop;
    int* resultPtr = NULL;
    pthread_join(core->thread, (void**)&resultPtr);
    int result = (resultPtr) ? *resultPtr : 0;
    free(resultPtr);
    free_Stack(core->stack);
    core->stack = NULL;
    return result;
}

static int pause_core(Core* core)
{
    core->status = Core_Paused;
    int* resultPtr = NULL;
    pthread_join(core->thread, (void**)&resultPtr);
    int result = *resultPtr;
    free(resultPtr);
    return result;
}

static int resume_core(Core* core)
{
    core->status = Core_Running;
    atomic_fetch_add(&(core->executor->runningCores), 1);
    return pthread_create(&core->thread, NULL, exec_Core, (void*)core);
}

Core* new_Core(Executor *executor, ModuleInst* module, uint32_t startFuncAddr)
{
    Core *core = (Core *) malloc(sizeof(Core));
    core->executor = executor;
    core->stack = NULL;
    core->startFuncAddr = startFuncAddr;
    core->status = Core_Stop;
    core->module = module;
    core->run = run_Core;
    core->stop = stop_core;
    core->pause = pause_core;
    core->resume = resume_core;
    return core;
}

void clean_Core(Core *core)
{
    if(core->status != Core_Stop) {
        core->stop(core);
    }
}

void free_Core(Core* core)
{
    clean_Core(core);
    free(core);
}