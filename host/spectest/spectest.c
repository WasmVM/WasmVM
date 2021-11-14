/**
 * Copyright 2021 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include "spectest.h"

wasm_module_inst spectest_instanciate(wasm_store store)
{
    wasm_module_inst module = module_inst_create();
    module->exports.size = 4;
    vector_resize(module->exports, wasm_export_inst, 4);
    wasm_export_inst* exports = module->exports.data;

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
        .value.i32 = 0,
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
        .value.i64 = 0,
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

    // (table (export "table") 10 20 funcref)
    // (memory (export "memory") 1 2)
    // (func (export "print"))
    // (func (export "print_i32") (param i32))
    // (func (export "print_i64") (param i64))
    // (func (export "print_f32") (param f32))
    // (func (export "print_f64") (param f64))
    // (func (export "print_i32_f32") (param i32 f32))
    // (func (export "print_f64_f64") (param f64 f64))
    return module;
}
