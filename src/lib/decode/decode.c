/**
 * Copyright 2021 Luis Hsu, Kevin Cyu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include <WasmVM.h>
#include <defines.h>
#include <error.h>
#include <structures/WasmModule.h>

#include "sections.h"

wasm_module module_decode(const bytes_vector_t bytes)
{
    wasmvm_errno = ERROR_success;
    wasm_module module = NULL;
    // Allocate module memory
    if((module = (WasmModule*)malloc_func(sizeof(WasmModule))) == NULL) {
        wasmvm_errno = ERROR_undefined;
        return NULL;
    }
    // Initialize
    vector_init(module->types);
    vector_init(module->imports);
    vector_init(module->funcs);
    vector_init(module->tables);
    vector_init(module->mems);
    vector_init(module->globals);
    vector_init(module->exports);
    vector_init(module->elems);
    vector_init(module->datas);
    module->start = -1;

    // Cursor pointers
    const byte_t* read_p = bytes.data;
    const byte_t* end_p = bytes.data + bytes.size;

    // Magic number & version
    if(parse_magic_version(&read_p, end_p)) {
        module_free(module);
        return NULL;
    }

    // Section 1: Type
    if(skip_custom_section(&read_p, end_p)) {
        module_free(module);
        return NULL;
    }
    if(parse_type_section(module, &read_p, end_p)) {
        module_free(module);
        return NULL;
    }

    // Section 2: Import
    if(skip_custom_section(&read_p, end_p)) {
        module_free(module);
        return NULL;
    }
    if(parse_import_section(module, &read_p, end_p)) {
        module_free(module);
        return NULL;
    }

    // Section 3: Func
    if(skip_custom_section(&read_p, end_p)) {
        module_free(module);
        return NULL;
    }
    if(parse_func_section(module, &read_p, end_p)) {
        module_free(module);
        return NULL;
    }

    // Section 4: Table
    if(skip_custom_section(&read_p, end_p)) {
        module_free(module);
        return NULL;
    }
    if(parse_table_section(module, &read_p, end_p)) {
        module_free(module);
        return NULL;
    }

    // Section 5: Memory
    if(skip_custom_section(&read_p, end_p)) {
        module_free(module);
        return NULL;
    }
    if(parse_memory_section(module, &read_p, end_p)) {
        module_free(module);
        return NULL;
    }

    // Section 6: Global
    if(skip_custom_section(&read_p, end_p)) {
        module_free(module);
        return NULL;
    }
    if(parse_global_section(module, &read_p, end_p)) {
        module_free(module);
        return NULL;
    }

    // Section 7: Export
    if(skip_custom_section(&read_p, end_p)) {
        module_free(module);
        return NULL;
    }
    if(parse_export_section(module, &read_p, end_p)) {
        module_free(module);
        return NULL;
    }

    // Section 8: Start
    if(skip_custom_section(&read_p, end_p)) {
        module_free(module);
        return NULL;
    }
    if(parse_start_section(module, &read_p, end_p)) {
        module_free(module);
        return NULL;
    }

    // Section 9: Element
    if(skip_custom_section(&read_p, end_p)) {
        module_free(module);
        return NULL;
    }
    if(parse_element_section(module, &read_p, end_p)) {
        module_free(module);
        return NULL;
    }

    // Section 12: Data count
    if(skip_custom_section(&read_p, end_p)) {
        module_free(module);
        return NULL;
    }
    if(parse_data_count_section(module, &read_p, end_p)) {
        module_free(module);
        return NULL;
    }

    // Section 10: Code
    if(skip_custom_section(&read_p, end_p)) {
        module_free(module);
        return NULL;
    }
    if(parse_code_section(module, &read_p, end_p)) {
        module_free(module);
        return NULL;
    }

    // Section 11: Data
    if(skip_custom_section(&read_p, end_p)) {
        module_free(module);
        return NULL;
    }
    if(parse_data_section(module, &read_p, end_p)) {
        module_free(module);
        return NULL;
    }

    // Malformed section num
    if(read_p < end_p) {
        if(*read_p >= 12) {
            // Malformed section num
            wasmvm_errno = ERROR_malform_sec_id;
        } else {
            // Junk
            wasmvm_errno = ERROR_junk_aft_sect;
        }
        module_free(module);
        return NULL;
    }
    return module;
}