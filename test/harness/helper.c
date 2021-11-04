/**
 * Copyright 2021 Luis Hsu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */
#include <helper.h>

#include <stdio.h>
#include <stdlib.h>
#include <error.h>

byte_t* load_file(const char* filename, size_t* size)
{
    FILE* fin = NULL;
    // Open file
    if((fin = fopen(filename, "rb")) == NULL) {
        return NULL;
    }
    // Get file size
    if(fseek(fin, 0L, SEEK_END)) {
        return NULL;
    }
    *size = ftell(fin);
    if(*size == -1L) {
        return NULL;
    }
    if(fseek(fin, 0L, SEEK_SET)) {
        return NULL;
    }
    // Allocate data memory
    byte_t* data = malloc(*size);
    if(data == NULL) {
        return NULL;
    }
    // Read file
    if(fread(data, sizeof(char), *size, fin) != *size) {
        return NULL;
    }
    fclose(fin);
    return data;
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

int match_imports(const wasm_module module, const struct _hashmap* moduleInsts, size_t* size, ExternVal** externvals)
{
    // Get imports
    ExternType* imports = NULL;
    if(module_imports(module, size, &imports)) {
        *externvals = NULL;
        *size = 0;
        return -1;
    }
    // Allocate externVals
    *externvals = (ExternVal*) malloc_func(sizeof(ExternVal) * (*size));
    // Retrieve externvals
    for(size_t impIdx = 0; impIdx < *size; ++impIdx) {
        // Get module_inst
        wasm_module_inst moduleInst = hashmap_get(
                                          ModuleInst,
                                          sizeof(byte_t) * imports[impIdx].module.size,
                                          imports[impIdx].module.data,
                                          moduleInsts
                                      );
        if(moduleInst == NULL) {
            // Module not found
            free(*externvals);
            *externvals = NULL;
            *size = 0;
            wasmvm_errno = ERROR_unknown_import;
            return -1;
        }
        // Get export
        ExternVal* exportValPtr = NULL;
        for(unsigned int expIdx = 0; expIdx < moduleInst->exports.size; ++expIdx) {
            if(name_compare(
                        moduleInst->exports.data[expIdx].name.size,
                        moduleInst->exports.data[expIdx].name.data,
                        imports[impIdx].name.size,
                        imports[impIdx].name.data
                    )) {
                exportValPtr = &(moduleInst->exports.data[expIdx].value);
                break;
            }
        }
        if(exportValPtr == NULL) {
            // Export not found
            free(*externvals);
            *externvals = NULL;
            *size = 0;
            wasmvm_errno = ERROR_unknown_import;
            return -1;
        } else {
            (*externvals)[impIdx] = *exportValPtr;
        }
    }
    return 0;
}