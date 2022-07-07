/**
 * Copyright 2022 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include "validate.h"
#include "stack.h"

_Bool func_validate(WasmFunc* func){
    value_stack_t value_stack = {.size = 0, .data = NULL};
    ctrl_stack_t ctrl_stack = NULL;
    for(size_t i = 0; i < func->body.size; ++i){
        
    }
    return 1;
}