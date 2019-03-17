#include <core/Core.h>
#include <stdint.h>
#include <stdlib.h>

Core* new_Core(Store *thisStore)
{
    Core *core = (Core *) malloc(sizeof(Core));
    core->thisStore = thisStore;

    return core;
}

void free_Core(Core* thisCore)
{
    // free the current Core only, since Store and Stack may be used by other core.
    free(thisCore);
}

static void run_control_runtime_func(Stack *stack, ControlInstrInst *controlInstrInst, uint8_t opcode)
{

}

static void run_control_runtime_func(Stack *stack, ParametricInstrInst *parametricInstrInst, uint8_t opcode)
{
    switch (opcode) {
        case Op_drop:
            runtime_drop(stack);
            break;
        case Op_select:
            runtime_select(stack);
            break;
        default:
            break;
    }
}

void run_Core(Core *core, ModuleInst *moduleInst, FuncInst *startFunc)
{
    core->moduleInst = moduleInst;
    core->thisStack = new_Stack(NULL);

    /*
    2. set Func instance with func in Store, but the Func Core called is startFunc, whose FuncInst is one of the args.
    Therefore, we don't have to get a FuncInst of startFunc which we already have.
    FuncInst *funcInst = core->thisStore->funcs->at(core->thisStore->funcs, moduleInst->funcaddrs->at(moduleInst->funcaddrs, a));
    The `a` in "core->thisStore->funcs->at(core->thisStore->funcs, moduleInst->funcaddrs->at(moduleInst->funcaddrs, `a`))" is the "Invocation of function address a" in Spec
    link: https://webassembly.github.io/spec/core/exec/instructions.html#invocation-of-function-address
    */

    // 3. set func type
    FuncType *funcType = new_FuncType();
    memcpy(funcType, startFunc->type);

    // 5. set local value type of func
    vector *localValType = new_vector(sizeof(ValueType), NULL);
    memcpy(localValType, startFunc->locals);

    // 6. insert end
    startFunc->code->push_back(startFunc->code, Op_end);

    /*
    8. Function call need to pop Value(val0*) from Stack, but here Core calls Start function which has no input argument.
    Therefore, it's needless to pop Value
    */

    // 9. set vector of local values to zero value
    vector *localVals = new_vector(sizeof(Value), NULL);
    //TODO push all the valueType in funcType onto localVals with assigned funcType and 0 value
    size_t len = localValType->length;
    for (size_t i = 0; i < len; i++) {
        localVals->push_back(localVals, localValType->at(localValType, i));
    }

    // 10. setup Frame of startFunc
    Frame *startFrame = new_Frame(moduleInst);
    memcpy(startFrame->moduleInst, moduleInst);
    startFrame->locals = localVals;

    // 11. push the addres of Frame onto Stack
    // TODO push Frame onto Stack with "arity m" which is missing here
    core->thisStack->entries->push(core->thisStack->entries, startFrame);

    // execute the instructions
    // this TODO is wrong: TODO traverse all the instrInst in FuncInst->code
    vector *code = startFunc->code;
    size_t inst_len = code->length;
    InstrInst *instr;
    for (int i = 0; i < inst_len; i++) {
        instr = code->at(code, i);
        // process opcode
        switch(instr->opcode) {
            // 收到 InstrInst 之後 opcode 得到 型態, 強制轉換成其他NumericInstrInst
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
                ControlInstrInst *controlInstrInst = (ControlInstrInst*)instr;
                run_control_runtime_func(core->thisStack, controlInstrInst, instr->opcode);
                break;
            case Op_drop:
            case Op_select:
                ParametricInstrInst *parametricInstrInst = (ParametricInstrInst*)instr;
                run_control_runtime_func(core->thisStack, parametricInstrInst, instr->opcode);
                break;
            case Op_get_local:
            case Op_set_local:
            case Op_tee_local:
            case Op_get_global:
            case Op_set_global:
                VariableInstrInst *variableInstrInst = (VariableInstrInst*)instr;
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
                MemoryInstrInst *memoryInstrInst = (MemoryInstrInst*)instr;
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
                NumericInstrInst *numericInstrInst = (NumericInstrInst*)instr;
                break;
            default:
                break;

        }

    }
}