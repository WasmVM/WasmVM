/**
 * Copyright 2022 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include <WasmVM.h>
#include "validate.h"

_Bool module_validate(wasm_module module){
    // Functions
    for(size_t i = 0; i < module->funcs.size; ++i){
        if(!func_validate(module->funcs.data + i)){
            return 0;
        }
    }
    return 1;
}