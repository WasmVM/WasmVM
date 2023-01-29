// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "dump.hpp"

std::ostream& WasmVM::operator<<(std::ostream& stream, WasmImport& import){
    stream << "  (import " << import.module << " " << import.name;
    stream << " )" << std::endl;
    return stream;
}