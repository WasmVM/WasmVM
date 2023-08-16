// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <decode.hpp>
#include "exception.hpp"

using namespace WasmVM;
using namespace Decode;

Stream& Decode::Start::read(Stream& stream){
    auto size = header(stream);
    if(size){
        stream >> start.emplace();
    }
    return stream;
}