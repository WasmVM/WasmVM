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

int module_decode(const byte_t* data, const size_t data_size, WasmModule** module)
{
    // Allocate module memory
    if((*module = (WasmModule*)malloc_func(sizeof(WasmModule))) == NULL) {
        return -1;
    }

    // Cursor pointers
    const byte_t* read_p = data;
    const byte_t* end_p = data + data_size;

    // Magic number & version
    if(parse_magic_version(&read_p, end_p)) {
        return -1;
    }

    // Section 1: Type
    if(skip_custom_section(&read_p, end_p)) {
        return -1;
    }
    if(parse_type_section(*module, &read_p, end_p)) {
        return -1;
    }

    // Malformed section num
    if((read_p < end_p) && (*read_p >= 12)) {
        wasmvm_errno = ERROR_malform_sec_id;
        return -1;
    }
    return ERROR_success;
}