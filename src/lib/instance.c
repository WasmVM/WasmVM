/**
 * Copyright 2022 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include <WasmVM.h>

wasm_externval instance_export(wasm_module_inst module_inst, const unsigned name_size, const byte_t name[name_size]){
    for(unsigned i = 0; i < module_inst->exports.size; ++i){
        wasm_export_inst* export = (module_inst->exports.data + i);
        if(export->name.size == name_size){
            _Bool matched = 1;
            for(unsigned c = 0; c < export->name.size; ++c){
                if(export->name.data[c] != name[c]){
                    matched = 0;
                    break;
                }
            }
            if(matched){
                return export->value;
            }
        }
    }
    wasmvm_errno = ERROR_unknown_import;
    wasm_externval empty;
    empty.type = Desc_Unspecified;
    return empty;
}