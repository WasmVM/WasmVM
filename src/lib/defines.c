/**
 * Copyright 2021 Luis Hsu, Kevin Cyu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include <defines.h>

void memcpy_func(char* const dst, const char* const src, u64_t size)
{
    for(u64_t i = 0; i < size; ++i) {
        dst[i] = src[i];
    }
}