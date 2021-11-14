/**
 * Copyright 2021 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */
#include <helper.h>

#include <stdio.h>
#include <stdlib.h>
#include <error.h>

bytes_vector_t load_file(const char* filename)
{
    FILE* fin = NULL;
    bytes_vector_t bytes;
    vector_init(bytes);
    // Open file
    if((fin = fopen(filename, "rb")) == NULL) {
        return bytes;
    }
    // Get file size
    if(fseek(fin, 0L, SEEK_END)) {
        return bytes;
    }
    long fileSize = ftell(fin);
    if(fileSize == -1L) {
        return bytes;
    } else {
        bytes.size = fileSize;
    }
    if(fseek(fin, 0L, SEEK_SET)) {
        return bytes;
    }
    // Allocate data memory
    bytes.data = malloc(bytes.size);
    if(bytes.data == NULL) {
        return bytes;
    }
    // Read file
    if(fread(bytes.data, sizeof(byte_t), bytes.size, fin) != bytes.size) {
        free_vector(bytes);
        bytes.data = NULL;
        return bytes;
    }
    fclose(fin);
    return bytes;
}

static _Bool name_compare(
    const size_t size1, const byte_t name1[size1],
    const size_t size2, const byte_t name2[size2]
)
{
    if(size1 != size2) {
        return 0;
    }
    for(size_t i = 0; i < size1; ++i) {
        if(name1[i] != name2[i]) {
            return 0;
        }
    }
    return 1;
}

externval_vector_t match_imports(const wasm_module module, const struct _hashmap* moduleInsts)
{
    externval_vector_t externals;
    vector_init(externals);
    // Get imports
    imports_vector_t imports = module_imports(module);
    if(wasmvm_errno) {
        wasmvm_errno = ERROR_unknown_import;
        return externals;
    }
    // Allocate externVals
    externals.size = imports.size;
    externals.data = (ExternVal*) malloc_func(sizeof(ExternVal) * imports.size);
    // Retrieve externvals
    for(size_t impIdx = 0; impIdx < imports.size; ++impIdx) {
        // Get module_inst
        wasm_module_inst moduleInst;
        hashmap_get(
            sizeof(byte_t) * imports.data[impIdx].module.size,
            imports.data[impIdx].module.data,
            moduleInst,
            moduleInsts
        );
        if(moduleInst == NULL) {
            // Module not found
            free_vector(externals);
            vector_init(externals);
            wasmvm_errno = ERROR_unknown_import;
            return externals;
        }
        // Get export
        ExternVal* exportValPtr = NULL;
        for(unsigned int expIdx = 0; expIdx < moduleInst->exports.size; ++expIdx) {
            if(name_compare(
                        moduleInst->exports.data[expIdx].name.size,
                        moduleInst->exports.data[expIdx].name.data,
                        imports.data[impIdx].name.size,
                        imports.data[impIdx].name.data
                    )) {
                exportValPtr = &(moduleInst->exports.data[expIdx].value);
                break;
            }
        }
        if(exportValPtr == NULL) {
            // Export not found
            free_vector(externals);
            vector_init(externals);
            wasmvm_errno = ERROR_unknown_import;
            return externals;
        } else {
            externals.data[impIdx] = *exportValPtr;
        }
    }
    return externals;
}