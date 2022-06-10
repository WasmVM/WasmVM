/**
 * Copyright 2021 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include "spectest.h"

#include <stdio.h>

static int spectest_print(wasm_stack* stack, Store* store)
{
    printf("[Spectest]\n");
    return 0;
}
static int spectest_print_i32(wasm_stack* stack, Store* store)
{
    // (func (export "print_i32") (param i32))
    wasm_stack value = *stack;
    *stack = (*stack)->next;
    printf("[Spectest] (i32) %d\n", value->entry.value.value.i32);
    free_func(value);
    return 0;
}
static int spectest_print_i64(wasm_stack* stack, Store* store)
{
    // (func (export "print_i64") (param i64))
    wasm_stack value = *stack;
    *stack = (*stack)->next;
    printf("[Spectest] (i64) %ld\n", value->entry.value.value.i64);
    free_func(value);
    return 0;
}
static int spectest_print_f32(wasm_stack* stack, Store* store)
{
    // (func (export "print_f32") (param f32))
    wasm_stack value = *stack;
    *stack = (*stack)->next;
    printf("[Spectest] (f32) %f\n", value->entry.value.value.f32);
    free_func(value);
    return 0;
}
static int spectest_print_f64(wasm_stack* stack, Store* store)
{
    // (func (export "print_f64") (param f64))
    wasm_stack value = *stack;
    *stack = (*stack)->next;
    printf("[Spectest] (f64) %lf\n", value->entry.value.value.f64);
    free_func(value);
    return 0;
}
static int spectest_print_i32_f32(wasm_stack* stack, Store* store)
{
    // (func (export "print_i32_f32") (param i32 f32))
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    *stack = value1->next;
    printf("[Spectest] (i32 f32) %d %lf\n", value1->entry.value.value.i32, value2->entry.value.value.f32);
    free_func(value1);
    free_func(value2);
    return 0;
}
static int spectest_print_f64_f64(wasm_stack* stack, Store* store)
{
    // (func (export "print_f64_f64") (param f64 f64))
    wasm_stack value2 = *stack;
    wasm_stack value1 = value2->next;
    *stack = value1->next;
    printf("[Spectest] (f64 f64) %lf %lf\n", value1->entry.value.value.f64, value2->entry.value.value.f64);
    free_func(value1);
    free_func(value2);
    return 0;
}

wasm_module_inst spectest_instanciate(wasm_store store)
{
    wasm_module_inst module = module_inst_create();

    // Allocate exports
    module->exports.size = 13;
    vector_resize(module->exports, wasm_export_inst, 13);
    wasm_export_inst* exports = module->exports.data;

    /** Types **/
    module->types.size = 7;
    vector_resize(module->types, wasm_functype, 7);

    // ()
    vector_init(module->types.data[0].params);
    vector_init(module->types.data[0].results);
    // (i32)
    vector_init(module->types.data[1].params);
    module->types.data[1].params.size = 1;
    vector_resize(module->types.data[1].params, ValueType, 1);
    module->types.data[1].params.data[0] = Value_i32;
    vector_init(module->types.data[1].results);
    // (i64)
    vector_init(module->types.data[2].params);
    module->types.data[2].params.size = 1;
    vector_resize(module->types.data[2].params, ValueType, 1);
    module->types.data[2].params.data[0] = Value_i64;
    vector_init(module->types.data[2].results);
    // (f32)
    vector_init(module->types.data[3].params);
    module->types.data[3].params.size = 1;
    vector_resize(module->types.data[3].params, ValueType, 1);
    module->types.data[3].params.data[0] = Value_f32;
    vector_init(module->types.data[3].results);
    // (f64)
    vector_init(module->types.data[4].params);
    module->types.data[4].params.size = 1;
    vector_resize(module->types.data[4].params, ValueType, 1);
    module->types.data[4].params.data[0] = Value_f64;
    vector_init(module->types.data[4].results);
    // (i32 f32)
    vector_init(module->types.data[5].params);
    module->types.data[5].params.size = 2;
    vector_resize(module->types.data[5].params, ValueType, 2);
    module->types.data[5].params.data[0] = Value_i32;
    module->types.data[5].params.data[1] = Value_f32;
    vector_init(module->types.data[5].results);
    // (f64 f64)
    vector_init(module->types.data[6].params);
    module->types.data[6].params.size = 2;
    vector_resize(module->types.data[6].params, ValueType, 2);
    module->types.data[6].params.data[0] = Value_f64;
    module->types.data[6].params.data[1] = Value_f64;
    vector_init(module->types.data[6].results);

    /** Globals **/
    module->globaladdrs.size = 4;
    vector_resize(module->globaladdrs, u32_t, 4);

    // (global (export "global_i32") i32)
    module->globaladdrs.data[0] = global_alloc(store,
    (wasm_global) {
        .init = {
            .type = Const_Value,
            .value = {
                .type = Value_i32,
                .value.i32 = 0,
            },
        },
        .mut = 1,
        .valType = Value_i32,
    },
    (wasm_value) {
        .type = Value_i32,
        .value.i32 = 666,
    }
                                              );
    exports[0].name.size = 10;
    exports[0].name.data = (byte_t*)malloc_func(sizeof(byte_t) * 10);
    memcpy_func(exports[0].name.data, "global_i32", sizeof(byte_t) * 10);
    exports[0].value.type = Desc_Global;
    exports[0].value.value = module->globaladdrs.data[0];

    // (global (export "global_i64") i64)
    module->globaladdrs.data[1] = global_alloc(store,
    (wasm_global) {
        .init = {
            .type = Const_Value,
            .value = {
                .type = Value_i64,
                .value.i64 = 0,
            },
        },
        .mut = 1,
        .valType = Value_i64,
    },
    (wasm_value) {
        .type = Value_i64,
        .value.i64 = 666,
    }
                                              );
    exports[1].name.size = 10;
    exports[1].name.data = (byte_t*)malloc_func(sizeof(byte_t) * 10);
    memcpy_func(exports[1].name.data, "global_i64", sizeof(byte_t) * 10);
    exports[1].value.type = Desc_Global;
    exports[1].value.value = module->globaladdrs.data[1];

    // (global (export "global_f32") f32)
    module->globaladdrs.data[2] = global_alloc(store,
    (wasm_global) {
        .init = {
            .type = Const_Value,
            .value = {
                .type = Value_f32,
                .value.f32 = 0,
            },
        },
        .mut = 1,
        .valType = Value_f32,
    },
    (wasm_value) {
        .type = Value_f32,
        .value.f32 = 0,
    }
                                              );
    exports[2].name.size = 10;
    exports[2].name.data = (byte_t*)malloc_func(sizeof(byte_t) * 10);
    memcpy_func(exports[2].name.data, "global_f32", sizeof(byte_t) * 10);
    exports[2].value.type = Desc_Global;
    exports[2].value.value = module->globaladdrs.data[2];

    // (global (export "global_f64") f64)
    module->globaladdrs.data[3] = global_alloc(store,
    (wasm_global) {
        .init = {
            .type = Const_Value,
            .value = {
                .type = Value_f64,
                .value.f64 = 0,
            },
        },
        .mut = 1,
        .valType = Value_f64,
    },
    (wasm_value) {
        .type = Value_f64,
        .value.f64 = 0,
    }
                                              );
    exports[3].name.size = 10;
    exports[3].name.data = (byte_t*)malloc_func(sizeof(byte_t) * 10);
    memcpy_func(exports[3].name.data, "global_f64", sizeof(byte_t) * 10);
    exports[3].value.type = Desc_Global;
    exports[3].value.value = module->globaladdrs.data[0];

    /** Tables **/
    module->tableaddrs.size = 1;
    vector_resize(module->tableaddrs, u32_t, 1);

    // (table (export "table") 10 20 funcref)
    module->tableaddrs.data[0] = table_alloc(store,
    (wasm_table) {
        .min = 10,
        .max = 20,
        .refType = Ref_func,
    },
    (wasm_ref) {
        .addr = 0,
        .type = Ref_func,
        .isNull = 1,
    }
                                            );
    exports[4].name.size = 5;
    exports[4].name.data = (byte_t*)malloc_func(sizeof(byte_t) * 5);
    memcpy_func(exports[4].name.data, "table", sizeof(byte_t) * 5);
    exports[4].value.type = Desc_Table;
    exports[4].value.value = module->tableaddrs.data[0];

    /** Memories **/
    module->memaddrs.size = 1;
    vector_resize(module->memaddrs, u32_t, 1);

    // (memory (export "memory") 1 2)
    module->memaddrs.data[0] = mem_alloc(store,
    (wasm_memory) {
        .min = 1,
        .max = 2,
    }
                                        );
    exports[5].name.size = 6;
    exports[5].name.data = (byte_t*)malloc_func(sizeof(byte_t) * 6);
    memcpy_func(exports[5].name.data, "memory", sizeof(byte_t) * 6);
    exports[5].value.type = Desc_Mem;
    exports[5].value.value = module->memaddrs.data[0];

    /** Functions **/
    module->funcaddrs.size = 7;
    vector_resize(module->funcaddrs, u32_t, 7);
    ValueType* params = NULL;
    ValueType* results = NULL;

    // (func (export "print"))
    module->funcaddrs.data[0] = func_alloc(store,
                                           module->types.data,
                                           spectest_print
                                          );
    exports[6].name.size = 5;
    exports[6].name.data = (byte_t*)malloc_func(sizeof(byte_t) * 5);
    memcpy_func(exports[6].name.data, "print", sizeof(byte_t) * 5);
    exports[6].value.type = Desc_Func;
    exports[6].value.value = module->funcaddrs.data[0];

    // (func (export "print_i32") (param i32))
    module->funcaddrs.data[1] = func_alloc(store,
                                           module->types.data + 1,
                                           spectest_print_i32
                                          );
    exports[7].name.size = 9;
    exports[7].name.data = (byte_t*)malloc_func(sizeof(byte_t) * 9);
    memcpy_func(exports[7].name.data, "print_i32", sizeof(byte_t) * 9);
    exports[7].value.type = Desc_Func;
    exports[7].value.value = module->funcaddrs.data[1];

    // (func (export "print_i64") (param i64))
    module->funcaddrs.data[2] = func_alloc(store,
                                           module->types.data + 2,
                                           spectest_print_i64
                                          );
    exports[8].name.size = 9;
    exports[8].name.data = (byte_t*)malloc_func(sizeof(byte_t) * 9);
    memcpy_func(exports[8].name.data, "print_i64", sizeof(byte_t) * 9);
    exports[8].value.type = Desc_Func;
    exports[8].value.value = module->funcaddrs.data[2];

    // (func (export "print_f32") (param f32))
    module->funcaddrs.data[3] = func_alloc(store,
                                           module->types.data + 3,
                                           spectest_print_f32
                                          );
    exports[9].name.size = 9;
    exports[9].name.data = (byte_t*)malloc_func(sizeof(byte_t) * 9);
    memcpy_func(exports[9].name.data, "print_f32", sizeof(byte_t) * 9);
    exports[9].value.type = Desc_Func;
    exports[9].value.value = module->funcaddrs.data[3];

    // (func (export "print_f64") (param f64))
    module->funcaddrs.data[4] = func_alloc(store,
                                           module->types.data + 4,
                                           spectest_print_f64
                                          );
    exports[10].name.size = 9;
    exports[10].name.data = (byte_t*)malloc_func(sizeof(byte_t) * 9);
    memcpy_func(exports[10].name.data, "print_f64", sizeof(byte_t) * 9);
    exports[10].value.type = Desc_Func;
    exports[10].value.value = module->funcaddrs.data[4];

    // (func (export "print_i32_f32") (param i32 f32))
    module->funcaddrs.data[5] = func_alloc(store,
                                           module->types.data + 5,
                                           spectest_print_i32_f32
                                          );
    exports[11].name.size = 13;
    exports[11].name.data = (byte_t*)malloc_func(sizeof(byte_t) * 13);
    memcpy_func(exports[11].name.data, "print_i32_f32", sizeof(byte_t) * 13);
    exports[11].value.type = Desc_Func;
    exports[11].value.value = module->funcaddrs.data[5];

    // (func (export "print_f64_f64") (param f64 f64))
    module->funcaddrs.data[6] = func_alloc(store,
                                           module->types.data + 5,
                                           spectest_print_f64_f64
                                          );
    exports[12].name.size = 13;
    exports[12].name.data = (byte_t*)malloc_func(sizeof(byte_t) * 13);
    memcpy_func(exports[12].name.data, "print_f64_f64", sizeof(byte_t) * 13);
    exports[12].value.type = Desc_Func;
    exports[12].value.value = module->funcaddrs.data[6];

    return module;
}
