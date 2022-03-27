/**
 * Copyright 2022 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include <WasmVM.h>

values_vector_t func_invoke(wasm_store store, u32_t funcaddr, values_vector_t args){
    values_vector_t results;
    vector_init(results);
    return results;
}