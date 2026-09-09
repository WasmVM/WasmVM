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
        Expect(test_module.types[0].func() == void_type);
    })
    Test("param", {
        ParseFile(test_module, "param.wat");
        FuncType param_i32 = test_module.types[0].func();
        Expect(param_i32.params.size() == 1);
        Expect(param_i32.params[0] == ValueType::i32);
        FuncType param_i32_i64 = test_module.types[1].func();
        Expect(param_i32_i64.params.size() == 2);
        Expect(param_i32_i64.params[0] == ValueType::i32);
        Expect(param_i32_i64.params[1] == ValueType::i64);
        FuncType param_f32_f64 = test_module.types[2].func();
        Expect(param_f32_f64.params.size() == 2);
        Expect(param_f32_f64.params[0] == ValueType::f32);
        Expect(param_f32_f64.params[1] == ValueType::f64);
        Throw(Exception::Parse, {
            std::stringstream stream("(type (func (param $pdup i32) (param $pdup i64)))");
            module_parse(stream);
        });
        Throw(Exception::Parse, {
            std::stringstream stream("(type (func (param $pmul i32 i64)))");
            module_parse(stream);
        });
        Throw(Exception::Parse, {
            std::stringstream stream("(type (func (param $pnone)))");
            module_parse(stream);
        });
    })
    Test("result", {
        ParseFile(test_module, "result.wat");
        FuncType result_i32 = test_module.types[0].func();
        Expect(result_i32.results.size() == 1);
        Expect(result_i32.results[0] == ValueType::i32);
        FuncType param_i32_i64 = test_module.types[1].func();
        Expect(param_i32_i64.results.size() == 2);
        Expect(param_i32_i64.results[0] == ValueType::i32);
        Expect(param_i32_i64.results[1] == ValueType::i64);
        FuncType param_f32_f64 = test_module.types[2].func();
        Expect(param_f32_f64.results.size() == 2);
        Expect(param_f32_f64.results[0] == ValueType::f32);
        Expect(param_f32_f64.results[1] == ValueType::f64);
    })
    Test("param + result", {
        ParseFile(test_module, "param_result.wat");
        FuncType param_i32_result_f64 = test_module.types[0].func();
        Expect(param_i32_result_f64.params.size() == 1);
        Expect(param_i32_result_f64.params[0] == ValueType::i32);
        Expect(param_i32_result_f64.results.size() == 1);
        Expect(param_i32_result_f64.results[0] == ValueType::f64);
        FuncType param_i32_i64_result_i64 = test_module.types[1].func();
        Expect(param_i32_i64_result_i64.params.size() == 2);
        Expect(param_i32_i64_result_i64.params[0] == ValueType::i32);
        Expect(param_i32_i64_result_i64.params[1] == ValueType::i64);
        Expect(param_i32_i64_result_i64.results.size() == 1);
        Expect(param_i32_i64_result_i64.results[0] == ValueType::i64);
        FuncType param_f32_result_i64 = test_module.types[2].func();
        Expect(param_f32_result_i64.params.size() == 1);
        Expect(param_f32_result_i64.params[0] == ValueType::f32);
        Expect(param_f32_result_i64.results.size() == 1);
        Expect(param_f32_result_i64.results[0] == ValueType::i64);
    })
    Test("exnref", {
        std::stringstream stream("(module (type (func (param exnref) (result exnref))))");
        WasmModule test_module = module_parse(stream);
        FuncType exn_type = test_module.types[0].func();
        Expect(exn_type.params.size() == 1);
        Expect(exn_type.params[0] == ValueType::exnref);
        Expect(exn_type.results.size() == 1);
        Expect(exn_type.results[0] == ValueType::exnref);
    })
    Test("with id", {
        std::stringstream stream("(type $ty1 (func))");
        module_parse(stream);
    })
    Category("invalid", {
        Test("unknown param valtype", {
            Throw(Exception::Parse, {
                std::stringstream stream("(module (type (func (param bad))))");
                module_parse(stream);
            });
        })
        Test("unknown result valtype", {
            Throw(Exception::Parse, {
                std::stringstream stream("(module (type (func (result bad))))");
                module_parse(stream);
            });
        })
        Test("duplicate type id", {
            Throw(Exception::Parse, {
                std::stringstream stream("(module (type $t (func)) (type $t (func (param i32))))");
                module_parse(stream);
            });
        })
    })
    // GC type definitions: struct and array composite types, subtypes and
    // recursion groups. The interpreter does not execute any of these yet;
    // what is being pinned here is that the text form reads into the module
    // structure faithfully, since the binary and text writers build on it.
    Category("struct", {
        Test("fields, ids, packed storage and mutability", {
            ParseFile(test_module, "struct.wat");
            AssertEq(test_module.types.size(), 3U);
            Assert(test_module.types[0].is_struct());
            Expect(std::get<StructType>(test_module.types[0].comp).fields.empty());
            Expect(test_module.types[0].final);
            Expect(test_module.types[0].supers.empty());

            const StructType& point = std::get<StructType>(test_module.types[1].comp);
            AssertEq(point.fields.size(), 2U);
            Expect(point.fields[0].storage == StorageType(ValueType::i32));
            Expect(!point.fields[0].mut);

            const StructType& mixed = std::get<StructType>(test_module.types[2].comp);
            AssertEq(mixed.fields.size(), 4U);
            Expect(mixed.fields[0].storage == StorageType(PackedType::i8));
            Expect(!mixed.fields[0].mut);
            Expect(mixed.fields[1].storage == StorageType(PackedType::i16));
            Expect(mixed.fields[1].mut);
            Expect(mixed.fields[2].storage == StorageType(ValueType::f64));
            Expect(mixed.fields[2].mut);
            Expect(mixed.fields[3].storage == StorageType(ValueType::funcref));
        })
        Test("struct.get names a field by id", {
            std::stringstream stream(
                "(module (type $p (struct (field $x i32) (field $y i64)))"
                " (func struct.get $p $y struct.set $p $x struct.get_s $p 1))");
            WasmModule m = module_parse(stream);
            auto& body = m.funcs[0].body;
            Expect(body[0].opcode == Opcode::Struct_get);
            ExpectEq(std::get<WasmInstr::TwoIdx>(body[0].imm).b, 1U);
            Expect(body[1].opcode == Opcode::Struct_set);
            ExpectEq(std::get<WasmInstr::TwoIdx>(body[1].imm).b, 0U);
            ExpectEq(std::get<WasmInstr::TwoIdx>(body[2].imm).b, 1U);
        })
        Test("identical struct types are distinct types", {
            std::stringstream stream("(module (type (struct (field i32))) (type (struct (field i32))))");
            WasmModule m = module_parse(stream);
            ExpectEq(m.types.size(), 2U);
        })
        Category("invalid", {
            Test("duplicated field id", {
                Throw(Exception::Parse, {
                    std::stringstream stream("(module (type (struct (field $a i32) (field $a i32))))");
                    module_parse(stream);
                });
            })
            Test("unknown field id", {
                Throw(Exception::Parse, {
                    std::stringstream stream("(module (type $p (struct (field $x i32))) (func struct.get $p $nope))");
                    module_parse(stream);
                });
            })
            Test("a packed type is not a value type", {
                Throw(Exception::Parse, {
                    std::stringstream stream("(module (type (func (param i8))))");
                    module_parse(stream);
                });
            })
            Test("an inline typeuse cannot extend a struct type", {
                Throw(Exception::Parse, {
                    std::stringstream stream("(module (type $s (struct)) (func (type $s) (param i32)))");
                    module_parse(stream);
                });
            })
        })
    })
    Category("array", {
        Test("element storage and mutability", {
            ParseFile(test_module, "array.wat");
            AssertEq(test_module.types.size(), 3U);
            Assert(test_module.types[0].is_array());
            Expect(std::get<ArrayType>(test_module.types[0].comp).field.storage == StorageType(PackedType::i8));
            Expect(!std::get<ArrayType>(test_module.types[0].comp).field.mut);
            Expect(std::get<ArrayType>(test_module.types[1].comp).field.storage == StorageType(ValueType::i32));
            Expect(std::get<ArrayType>(test_module.types[1].comp).field.mut);
            Expect(std::get<ArrayType>(test_module.types[2].comp).field.storage == StorageType(ValueType::externref));
            Expect(std::get<ArrayType>(test_module.types[2].comp).field.mut);
        })
    })
    Category("sub", {
        Test("final, supertypes by id and by number", {
            ParseFile(test_module, "sub.wat");
            AssertEq(test_module.types.size(), 5U);
            Expect(!test_module.types[0].final);
            Expect(test_module.types[0].supers.empty());
            Expect(!test_module.types[1].final);
            AssertEq(test_module.types[1].supers.size(), 1U);
            ExpectEq(test_module.types[1].supers[0], 0U);
            Expect(test_module.types[2].final);
            AssertEq(test_module.types[2].supers.size(), 1U);
            ExpectEq(test_module.types[2].supers[0], 1U);
            AssertEq(test_module.types[3].supers.size(), 1U);
            ExpectEq(test_module.types[3].supers[0], 0U);
            // (sub final (func)) is the plain function type spelled out.
            Expect(test_module.types[4].final);
            Expect(test_module.types[4].supers.empty());
            Expect(test_module.types[4].is_func());
        })
        Test("a supertype id outside a rec group must already exist", {
            Throw(Exception::Parse, {
                std::stringstream stream("(module (type $a (sub $b (struct))) (type $b (struct)))");
                module_parse(stream);
            });
        })
    })
    Category("rec", {
        Test("groups, forward supertype ids, indices around them", {
            ParseFile(test_module, "rec.wat");
            AssertEq(test_module.types.size(), 6U);
            AssertEq(test_module.recs.size(), 2U);
            ExpectEq(test_module.recs[0].first, 1U);
            ExpectEq(test_module.recs[0].count, 3U);
            ExpectEq(test_module.recs[1].first, 4U);
            ExpectEq(test_module.recs[1].count, 1U);
            // $b names $c, which is defined after it in the same group.
            AssertEq(test_module.types[2].supers.size(), 1U);
            ExpectEq(test_module.types[2].supers[0], 3U);
            Expect(test_module.types[4].is_array());
            Expect(test_module.types[5].is_func());
            AssertEq(test_module.types[5].func().params.size(), 1U);
        })
        Test("a rec member is not folded into an identical plain type", {
            std::stringstream stream("(module (rec (type (func))) (type (func)))");
            WasmModule m = module_parse(stream);
            ExpectEq(m.types.size(), 2U);
            AssertEq(m.recs.size(), 1U);
            ExpectEq(m.recs[0].first, 0U);
        })
    })
    // Type deduplication drops repeated plain function types and shifts every
    // later index. The instructions that name a type index have to follow.
    Category("type indices survive deduplication", {
        Test("struct.new, struct.get and array.copy", {
            std::stringstream stream(
                "(module (type (func)) (type (func)) (type $s (struct (field i32))) (type $a (array i32))"
                " (func struct.new $s struct.get $s 0 array.copy $a $a ref.test (ref $s)))");
            WasmModule m = module_parse(stream);
            AssertEq(m.types.size(), 3U); // the two (func) folded into one
            auto& body = m.funcs[0].body;
            ExpectEq(std::get<WasmInstr::OneIdx>(body[0].imm).index, 1U);
            ExpectEq(std::get<WasmInstr::TwoIdx>(body[1].imm).a, 1U);
            ExpectEq(std::get<WasmInstr::TwoIdx>(body[2].imm).a, 2U);
            ExpectEq(std::get<WasmInstr::TwoIdx>(body[2].imm).b, 2U);
            ExpectEq(std::get<WasmInstr::HeapI32>(body[3].imm).heaptype, 1);
        })
        Test("call_ref and return_call_indirect", {
            std::stringstream stream(
                "(module (type (func)) (type (func)) (type $f (func (param i32))) (table 1 funcref)"
                " (func call_ref $f return_call_indirect (type $f)))");
            WasmModule m = module_parse(stream);
            AssertEq(m.types.size(), 2U);
            auto& body = m.funcs[0].body;
            ExpectEq(std::get<WasmInstr::OneIdx>(body[0].imm).index, 1U);
            ExpectEq(std::get<WasmInstr::TwoIdx>(body[1].imm).b, 1U);
        })
        Test("rec group bounds follow too", {
            std::stringstream stream("(module (type (func)) (type (func)) (rec (type (struct)) (type (struct))))");
            WasmModule m = module_parse(stream);
            AssertEq(m.types.size(), 3U);
            AssertEq(m.recs.size(), 1U);
            ExpectEq(m.recs[0].first, 1U);
            ExpectEq(m.recs[0].count, 2U);
        })
    })
};
