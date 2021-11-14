/**
 * Copyright 2021 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include <WasmVM.h>

imports_vector_t module_imports(wasm_module module)
{
    imports_vector_t imports;
    vector_init(imports);
    if(module->imports.size > 0) {
        imports.size = module->imports.size;
        imports.data = (ExternType*) malloc_func(sizeof(ExternType) * module->imports.size);
        for(size_t i = 0; i < imports.size; ++i) {
            WasmImport* moduleImport = module->imports.data + i;
            imports.data[i].module.size = moduleImport->module.size;
            imports.data[i].module.data = (byte_t*)malloc_func(sizeof(byte_t) * moduleImport->module.size);
            memcpy_func(imports.data[i].module.data, moduleImport->module.data, moduleImport->module.size);
            imports.data[i].name.size = moduleImport->name.size;
            imports.data[i].name.data = (byte_t*)malloc_func(sizeof(byte_t) * moduleImport->name.size);
            memcpy_func(imports.data[i].name.data, moduleImport->name.data, moduleImport->name.size);
            imports.data[i].descType = moduleImport->descType;
            switch (imports.data[i].descType) {
                case Desc_Func: {
                    FuncType funcType = module->types.data[moduleImport->desc.typeidx];
                    imports.data[i].desc.func.params.size = funcType.params.size;
                    imports.data[i].desc.func.params.data = malloc_func(sizeof(ValueType) * funcType.params.size);
                    memcpy_func(imports.data[i].desc.func.params.data, funcType.params.data, sizeof(ValueType) * funcType.params.size);
                    imports.data[i].desc.func.results.size = funcType.results.size;
                    imports.data[i].desc.func.results.data = malloc_func(sizeof(ValueType) * funcType.results.size);
                    memcpy_func(imports.data[i].desc.func.results.data, funcType.results.data, sizeof(ValueType) * funcType.results.size);
                }
                break;
                case Desc_Table:
                case Desc_Mem:
                    imports.data[i].desc.limits.max = moduleImport->desc.limits.max;
                    imports.data[i].desc.limits.min = moduleImport->desc.limits.min;
                    break;
                case Desc_Global:
                    imports.data[i].desc.global.mut = moduleImport->desc.global.mut;
                    imports.data[i].desc.global.valueType = moduleImport->desc.global.valueType;
                    break;
                default:
                    wasmvm_errno = ERROR_malform_import;
                    break;
            }
        }
    }
    return imports;
}