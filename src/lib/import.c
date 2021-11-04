/**
 * Copyright 2021 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include <WasmVM.h>

int module_imports(wasm_module module, size_t* size, ExternType** imports)
{
    *size = module->imports.size;
    *imports = (ExternType*) malloc_func(sizeof(ExternType) * module->imports.size);
    for(size_t i = 0; i < *size; ++i) {
        WasmImport* moduleImport = module->imports.data + i;
        (*imports)[i].module.data = (byte_t*)malloc_func(sizeof(byte_t) * moduleImport->module.size);
        memcpy_func((char*)((*imports)[i].module.data), (char*)moduleImport->module.data, moduleImport->module.size);
        (*imports)[i].name.data = (byte_t*)malloc_func(sizeof(byte_t) * moduleImport->name.size);
        memcpy_func((char*)((*imports)[i].name.data), (char*)moduleImport->name.data, moduleImport->name.size);
        (*imports)[i].descType = moduleImport->descType;
        switch ((*imports)[i].descType) {
            case Desc_Func: {
                FuncType funcType = module->types.data[moduleImport->desc.typeidx];
                (*imports)[i].desc.func.params.size = funcType.params.size;
                (*imports)[i].desc.func.params.data = malloc_func(sizeof(ValueType) * funcType.params.size);
                memcpy_func((char*)((*imports)[i].desc.func.params.data), (char*)funcType.params.data, sizeof(ValueType) * funcType.params.size);
                (*imports)[i].desc.func.results.size = funcType.results.size;
                (*imports)[i].desc.func.results.data = malloc_func(sizeof(ValueType) * funcType.results.size);
                memcpy_func((char*)((*imports)[i].desc.func.results.data), (char*)funcType.results.data, sizeof(ValueType) * funcType.results.size);
            }
            break;
            case Desc_Table:
            case Desc_Mem:
                (*imports)[i].desc.limits.max = moduleImport->desc.limits.max;
                (*imports)[i].desc.limits.min = moduleImport->desc.limits.min;
                break;
            case Desc_Global:
                (*imports)[i].desc.global.mut = moduleImport->desc.global.mut;
                (*imports)[i].desc.global.valueType = moduleImport->desc.global.valueType;
                break;
            default:
                // TODO: Error
                break;
        }
    }
    return 0;
}