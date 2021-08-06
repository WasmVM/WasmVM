/**
 * Copyright 2021 Luis Hsu, Kevin Cyu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include <WasmVM.h>
#include <defines.h>
#include <error.h>
#include <structures/WasmModule.h>

int module_decode(const char* data, const size_t data_size, WasmModule** module)
{
    // Allocate module memory
    if((*module = (WasmModule*)malloc_func(sizeof(WasmModule))) == NULL) {
        return -1;
    }

    // Cursor pointers
    const char* cursor_p = data;
    const char* end_p = data + data_size - 1;

    // Magic number & version
    if(parse_magic_version(module, &cursor_p)) {
        return -1;
    }
    return ERROR_success;
}