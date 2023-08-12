// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "numeric.hpp"

using namespace WasmVM;

#define Unop(Ty) \
    template<> Ty WasmVM::get_op<Ty>(Stack& stack){ \
        Label& label = stack.frames.top().labels.top(); \
        Ty value = std::get<Ty>(label.values.top()); \
        label.values.pop(); \
        return value; \
    }

#define Binop(Ty) \
    template<> std::pair<Ty, Ty> WasmVM::get_ops<Ty>(Stack& stack){ \
        Label& label = stack.frames.top().labels.top(); \
        Ty val2 = std::get<Ty>(label.values.top()); \
        label.values.pop(); \
        Ty val1 = std::get<Ty>(label.values.top()); \
        label.values.pop(); \
        return {val1, val2}; \
    }

void WasmVM::put_op(Stack& stack, Value&& value){
    stack.frames.top().labels.top().values.push(value);
}

Unop(i32_t)
Unop(i64_t)
Unop(f32_t)
Unop(f64_t)

Binop(i32_t)
Binop(i64_t)
Binop(f32_t)
Binop(f64_t)