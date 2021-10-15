/**
 * Copyright 2021 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include <error.h>
#include <Store.h>
#include <structures/WasmModule.h>
#include <instance/ExternVal.h>
#include <instance/ModuleInst.h>

int module_instantiate(wasm_store store, const wasm_module module, const size_t extern_size, const ExternVal extern_val[extern_size], wasm_module_inst* module_inst)
{
    // TODO:
    return ERROR_success;
}