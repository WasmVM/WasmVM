// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <harness.hpp>
#include <WasmVM.hpp>

#include <sstream>

using namespace WasmVM;
using namespace Testing;

Suite type {
    Test("void", {
        ParseFile(test_module, "void.wat");
        FuncType void_type;
        Expect(test_module.types[0] == void_type);
    })
    Test("param", {
        ParseFile(test_module, "param.wat");
        FuncType param_i32 = test_module.types[0];
        Expect(param_i32.params.size() == 1);
        Expect(param_i32.params[0] == ValueType::i32);
        FuncType param_i32_i64 = test_module.types[1];
        Expect(param_i32_i64.params.size() == 2);
        Expect(param_i32_i64.params[0] == ValueType::i32);
        Expect(param_i32_i64.params[1] == ValueType::i64);
        FuncType param_f32_f64 = test_module.types[2];
        Expect(param_f32_f64.params.size() == 2);
        Expect(param_f32_f64.params[0] == ValueType::f32);
        Expect(param_f32_f64.params[1] == ValueType::f64);
        Throw(Exception::Parse, {
            std::stringstream stream("(type (func (param $pdup i32 i64)))");
            module_parse(stream);
        });
        Throw(Exception::Parse, {
            std::stringstream stream("(type (func (param $pnone)))");
            module_parse(stream);
        });
    })
    Test("result", {
        ParseFile(test_module, "result.wat");
        FuncType result_i32 = test_module.types[0];
        Expect(result_i32.results.size() == 1);
        Expect(result_i32.results[0] == ValueType::i32);
        FuncType param_i32_i64 = test_module.types[1];
        Expect(param_i32_i64.results.size() == 2);
        Expect(param_i32_i64.results[0] == ValueType::i32);
        Expect(param_i32_i64.results[1] == ValueType::i64);
        FuncType param_f32_f64 = test_module.types[2];
        Expect(param_f32_f64.results.size() == 2);
        Expect(param_f32_f64.results[0] == ValueType::f32);
        Expect(param_f32_f64.results[1] == ValueType::f64);
    })
    Test("param + result", {
        ParseFile(test_module, "param_result.wat");
        FuncType param_i32_result_f64 = test_module.types[0];
        Expect(param_i32_result_f64.params.size() == 1);
        Expect(param_i32_result_f64.params[0] == ValueType::i32);
        Expect(param_i32_result_f64.results.size() == 1);
        Expect(param_i32_result_f64.results[0] == ValueType::f64);
        FuncType param_i32_i64_result_i64 = test_module.types[1];
        Expect(param_i32_i64_result_i64.params.size() == 2);
        Expect(param_i32_i64_result_i64.params[0] == ValueType::i32);
        Expect(param_i32_i64_result_i64.params[1] == ValueType::i64);
        Expect(param_i32_i64_result_i64.results.size() == 1);
        Expect(param_i32_i64_result_i64.results[0] == ValueType::i64);
        FuncType param_f32_result_i64 = test_module.types[2];
        Expect(param_f32_result_i64.params.size() == 1);
        Expect(param_f32_result_i64.params[0] == ValueType::f32);
        Expect(param_f32_result_i64.results.size() == 1);
        Expect(param_f32_result_i64.results[0] == ValueType::i64);
    })
    Test("with id", {
        std::stringstream stream("(type $ty1 (func))");
        module_parse(stream);
    })
};