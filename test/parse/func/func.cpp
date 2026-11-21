// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <harness.hpp>
#include <WasmVM.hpp>
#include <exception.hpp>

#include <sstream>
#include <string>
#include <vector>

using namespace WasmVM;
using namespace Testing;
using IndirectNameMap = WasmNames::IndirectNameMap;

// The message of the Exception::Parse that parsing `text` throws, or "" when
// it parses.
static std::string parse_error(const std::string& text) {
    std::istringstream source(text);
    try {
        module_parse(source);
    } catch (Exception::Parse& e) {
        return e.what();
    }
    return "";
}

Suite func {
    Test("normal", {
        ParseFile(test_module, "normal.wat");
        WasmFunc& func0 = test_module.funcs[0];
        Expect(func0.typeidx == 0);
        Expect(func0.locals.size() == 0);
        Expect(func0.body.size() == 1);
        WasmFunc& func1 = test_module.funcs[1];
        Expect(func1.typeidx == 0);
        Expect(func1.locals.size() == 0);
        Expect(func1.body.size() == 1);
        WasmFunc& func2 = test_module.funcs[2];
        Expect(func2.typeidx == 0);
        Expect(func2.locals.size() == 1);
        Expect(func2.locals[0] == ValueType::i32);
        Expect(func2.body.size() == 1);
        WasmFunc& func3 = test_module.funcs[3];
        Expect(func3.typeidx == 0);
        Expect(func3.locals.size() == 2);
        Expect(func3.locals[0] == ValueType::i32);
        Expect(func3.locals[1] == ValueType::i64);
        Expect(func3.body.size() == 1);
        WasmFunc& func4 = test_module.funcs[4];
        Expect(func4.typeidx == 0);
        Expect(func4.locals.size() == 2);
        Expect(func4.locals[0] == ValueType::f32);
        Expect(func4.locals[1] == ValueType::i64);
        Expect(func4.body.size() == 1);
        WasmFunc& func5 = test_module.funcs[5];
        Expect(func5.typeidx == 0);
        Expect(func5.locals.size() == 1);
        Expect(func5.locals[0] == ValueType::i32);
        Expect(func5.body.size() == 1);
    })
    Test("with_import", {
        ParseFile(test_module, "with_import.wat");
        WasmImport& imp0 = test_module.imports[0];
        Expect(std::holds_alternative<index_t>(imp0.desc));
        index_t& typeidx0 = std::get<index_t>(imp0.desc);
        Expect(imp0.module == "testmod");
        Expect(imp0.name == "func0");
        Expect(typeidx0 == 0);

        WasmImport& imp1 = test_module.imports[1];
        Expect(std::holds_alternative<index_t>(imp1.desc));
        index_t& typeidx1 = std::get<index_t>(imp1.desc);
        Expect(imp1.module == "testmod");
        Expect(imp1.name == "func1");
        Expect(typeidx1 == 0);

        WasmImport& imp2 = test_module.imports[2];
        Expect(std::holds_alternative<index_t>(imp2.desc));
        index_t& typeidx2 = std::get<index_t>(imp2.desc);
        Expect(imp2.module == "testmod");
        Expect(imp2.name == "func3");
        Expect(typeidx2 == 0);

    })
    Test("with_export", {
        ParseFile(test_module, "with_export.wat");

        WasmImport& imp0 = test_module.imports[0];
        Expect(std::holds_alternative<index_t>(imp0.desc));
        index_t& typeidxA = std::get<index_t>(imp0.desc);
        Expect(imp0.module == "testmod");
        Expect(imp0.name == "funcA");
        Expect(typeidxA == 0);

        WasmFunc& func0 = test_module.funcs[0];
        Expect(func0.typeidx == 0);
        Expect(func0.locals.size() == 1);
        Expect(func0.locals[0] == ValueType::i32);
        Expect(func0.body.size() == 1);

        WasmFunc& func1 = test_module.funcs[1];
        Expect(func1.typeidx == 0);
        Expect(func1.locals.size() == 1);
        Expect(func1.locals[0] == ValueType::i64);
        Expect(func1.body.size() == 1);

        WasmFunc& func2 = test_module.funcs[2];
        Expect(func2.typeidx == 0);
        Expect(func2.locals.size() == 1);
        Expect(func2.locals[0] == ValueType::f32);
        Expect(func2.body.size() == 1);

        WasmExport& export0 = test_module.exports[0];
        Expect(export0.name == "func0");
        Expect(export0.desc == WasmExport::DescType::func);
        Expect(export0.index == 0);

        WasmExport& export1 = test_module.exports[1];
        Expect(export1.name == "func1");
        Expect(export1.desc == WasmExport::DescType::func);
        Expect(export1.index == 1);

        WasmExport& export2 = test_module.exports[2];
        Expect(export2.name == "func2");
        Expect(export2.desc == WasmExport::DescType::func);
        Expect(export2.index == 2);

        WasmExport& export3 = test_module.exports[3];
        Expect(export3.name == "func3");
        Expect(export3.desc == WasmExport::DescType::func);
        Expect(export3.index == 2);

        WasmExport& export4 = test_module.exports[4];
        Expect(export4.name == "func4");
        Expect(export4.desc == WasmExport::DescType::func);
        Expect(export4.index == 3);
    })
    // A function's locals are indexed after its parameters, so `$a` in
    // (func (param $p i32) (local $a i32)) is local 1. The id map was filled
    // with the count of locals seen so far and no parameter offset, so the
    // first named local took index 0 -- the first parameter -- and every
    // later one was off by the same amount. With matching types nothing
    // complained: the function quietly read and wrote the wrong slot, and a
    // parameter could be clobbered by a write meant for a local (issue #20).
    Category("a named local is indexed after the parameters", {
        Test("one parameter, one named local", {
            std::istringstream source(
                "(module (func (param $p i32) (result i32) (local $a i32)"
                " i32.const 7 local.set $a local.get $a))");
            WasmModule module = module_parse(source);
            std::vector<index_t> indices;
            for (const WasmInstr& instr : module.funcs[0].body) {
                if (instr.opcode == Opcode::Local_get || instr.opcode == Opcode::Local_set) {
                    indices.push_back(std::get<WasmInstr::OneIdx>(instr.imm).index);
                }
            }
            AssertEq(indices.size(), 2U);
            ExpectEq(indices[0], 1U); // local.set $a
            ExpectEq(indices[1], 1U); // local.get $a
        })
        Test("a parameter id still names the parameter", {
            std::istringstream source(
                "(module (func (param $p i32) (result i32) (local $a i32)"
                " i32.const 7 local.set $a local.get $p))");
            WasmModule module = module_parse(source);
            const WasmInstr& last = module.funcs[0].body[2];
            Assert(last.opcode == Opcode::Local_get);
            ExpectEq(std::get<WasmInstr::OneIdx>(last.imm).index, 0U);
        })
        Test("several parameters and several locals", {
            std::istringstream source(
                "(module (func (param $p i32) (param $q i64) (result i32)"
                " (local $a f32) (local $b i32)"
                " local.get $p drop local.get $q drop local.get $a drop local.get $b))");
            WasmModule module = module_parse(source);
            std::vector<index_t> gets;
            for (const WasmInstr& instr : module.funcs[0].body) {
                if (instr.opcode == Opcode::Local_get) {
                    gets.push_back(std::get<WasmInstr::OneIdx>(instr.imm).index);
                }
            }
            AssertEq(gets.size(), 4U);
            ExpectEq(gets[0], 0U); // $p
            ExpectEq(gets[1], 1U); // $q
            ExpectEq(gets[2], 2U); // $a
            ExpectEq(gets[3], 3U); // $b
        })
        Test("one (local ...) clause declaring several types", {
            // Only the first of an id-less group is named, but the count
            // still has to advance by all of them.
            std::istringstream source(
                "(module (func (param $p i32) (result i32)"
                " (local i32 i64) (local $a f32) (local $b i32) local.get $b))");
            WasmModule module = module_parse(source);
            const WasmInstr& get = module.funcs[0].body[0];
            Assert(get.opcode == Opcode::Local_get);
            ExpectEq(std::get<WasmInstr::OneIdx>(get.imm).index, 4U);
        })
        Test("with no parameters the first local is still 0", {
            std::istringstream source(
                "(module (func (result i32) (local $a i32) i32.const 7 local.set $a local.get $a))");
            WasmModule module = module_parse(source);
            const WasmInstr& get = module.funcs[0].body[2];
            Assert(get.opcode == Opcode::Local_get);
            ExpectEq(std::get<WasmInstr::OneIdx>(get.imm).index, 0U);
        })
        Test("the types line up, so a mismatched use is now caught", {
            // $a is an f32 local behind an i32 parameter. Reading it as an
            // i32 used to succeed by hitting the parameter instead.
            std::istringstream source(
                "(module (func (param $p i32) (result i32) (local $a f32)"
                " local.get $a))");
            WasmModule module = module_parse(source);
            Expect(module_validate(module).has_value());
        })
    })
    // Inline params and results next to (type x) restate x; they used to be
    // appended to a copy of x, giving the function a new type (issue #60).
    Category("a typeuse with (type x) and inline params or results is x", {
        Test("inline params", {
            std::istringstream source(
                "(module (type $b (func (param i32))) (func (type $b) (param i32)))");
            WasmModule module = module_parse(source);
            ExpectEq(module.types.size(), 1U);
            AssertEq(module.funcs.size(), 1U);
            ExpectEq(module.funcs[0].typeidx, 0U);
        })
        Test("inline params, x inside a rec group", {
            std::istringstream source(
                "(module (rec (type $b (func (param i32)))) (func (type $b) (param i32)))");
            WasmModule module = module_parse(source);
            ExpectEq(module.types.size(), 1U);
            AssertEq(module.funcs.size(), 1U);
            ExpectEq(module.funcs[0].typeidx, 0U);
        })
        Test("inline results", {
            std::istringstream source(
                "(module (type $b (func (result i32))) (func (type $b) (result i32) i32.const 1))");
            WasmModule module = module_parse(source);
            ExpectEq(module.types.size(), 1U);
            AssertEq(module.funcs.size(), 1U);
            ExpectEq(module.funcs[0].typeidx, 0U);
            Expect(!module_validate(module).has_value());
        })
        Test("inline param ids name the locals, x's own ids do not", {
            std::istringstream source(
                "(module (type $b (func (param $p i32) (param $q i64)))"
                " (func (type $b) (param $x i32) (param i64) local.get $x drop))");
            WasmModule module = module_parse(source);
            AssertEq(module.funcs.size(), 1U);
            const WasmInstr& get = module.funcs[0].body[0];
            Assert(get.opcode == Opcode::Local_get);
            ExpectEq(std::get<WasmInstr::OneIdx>(get.imm).index, 0U);
            Expect((module.names.locals == IndirectNameMap{{0, {{0, "x"}}}}));
            ExpectEq(parse_error(
                "(module (type $b (func (param $p i32))) (func (type $b) (param $x i32) local.get $p drop))"),
                std::string("local id '$p' not found"));
        })
        Test("inline param ids name a tag's params", {
            std::istringstream source(
                "(module (type $t (func (param $p i32))) (tag (type $t) (param $a i32)))");
            WasmModule module = module_parse(source);
            ExpectEq(module.types.size(), 1U);
            AssertEq(module.tags.size(), 1U);
            ExpectEq(module.tags[0], 0U);
            Expect((module.names.tag_params == IndirectNameMap{{0, {{0, "a"}}}}));
        })
        Test("inline params that do not match x", {
            ExpectEq(parse_error("(module (type $b (func (param i32))) (func (type $b) (param f32)))"),
                     std::string("inline params/results do not match type 0"));
            ExpectEq(parse_error("(module (type $b (func (param i32))) (func (type $b) (param i32 i32)))"),
                     std::string("inline params/results do not match type 0"));
        })
        Test("inline results that do not match x", {
            ExpectEq(parse_error("(module (type (func)) (type $b (func (param i32))) (func (type $b) (param i32) (result i32) i32.const 1))"),
                     std::string("inline params/results do not match type 1"));
        })
        Test("x defined later", {
            std::istringstream source(
                "(module (func (type 0) (param $x i32) local.get $x drop) (type (func (param i32))))");
            WasmModule module = module_parse(source);
            ExpectEq(module.types.size(), 1U);
            AssertEq(module.funcs.size(), 1U);
            ExpectEq(module.funcs[0].typeidx, 0U);
            Expect(!module_validate(module).has_value());
        })
        Test("x never defined", {
            ExpectEq(parse_error("(module (func (type 1) (param i32)) (type (func (param i32))))"),
                     std::string("type 1 in typeuse is not defined yet"));
        })
    })
    // Declarations scope over the whole module, including the text before
    // them, so (type x) may name a type defined further down (issue #62).
    Category("a (type x) naming a type defined later is that type", {
        Test("an empty function type", {
            std::istringstream source("(module (func (type 0)) (type (func)))");
            WasmModule module = module_parse(source);
            ExpectEq(module.types.size(), 1U);
            AssertEq(module.funcs.size(), 1U);
            ExpectEq(module.funcs[0].typeidx, 0U);
            Expect(!module_validate(module).has_value());
        })
        Test("a named local is indexed after the later type's parameters", {
            std::istringstream source(
                "(module (func (type 0) (local $l i64) local.get $l drop) (type (func (param i32))))");
            WasmModule module = module_parse(source);
            AssertEq(module.funcs.size(), 1U);
            ExpectEq(module.funcs[0].typeidx, 0U);
            const WasmInstr& get = module.funcs[0].body[0];
            Assert(get.opcode == Opcode::Local_get);
            ExpectEq(std::get<WasmInstr::OneIdx>(get.imm).index, 1U);
            Expect(!module_validate(module).has_value());
        })
        Test("an index past the types defined so far", {
            std::istringstream source(
                "(module (type (func)) (func (type 1) (local $l i64) local.get $l drop)"
                " (type (func (param i32))))");
            WasmModule module = module_parse(source);
            AssertEq(module.funcs.size(), 1U);
            ExpectEq(module.funcs[0].typeidx, 1U);
            const WasmInstr& get = module.funcs[0].body[0];
            Assert(get.opcode == Opcode::Local_get);
            ExpectEq(std::get<WasmInstr::OneIdx>(get.imm).index, 1U);
            Expect(!module_validate(module).has_value());
        })
        Test("the later type's parameter ids", {
            std::istringstream source(
                "(module (func (type $t) local.get $p drop) (type $t (func (param $p i32))))");
            WasmModule module = module_parse(source);
            AssertEq(module.funcs.size(), 1U);
            ExpectEq(module.funcs[0].typeidx, 0U);
            const WasmInstr& get = module.funcs[0].body[0];
            Assert(get.opcode == Opcode::Local_get);
            ExpectEq(std::get<WasmInstr::OneIdx>(get.imm).index, 0U);
        })
        Test("a type that is never defined is the validator's to report", {
            std::istringstream source("(module (func (type 5)))");
            WasmModule module = module_parse(source);
            AssertEq(module.funcs.size(), 1U);
            ExpectEq(module.funcs[0].typeidx, 5U);
            Expect(module_validate(module).has_value());
        })
    })
    // Type definitions take the first type indices in the order written; a
    // type an inline typeuse inserts goes at the end of the module.
    Category("type definitions are numbered before inserted types", {
        Test("a definition after an inline typeuse is still type 0", {
            std::istringstream source(
                "(module (func $a (param i32)) (type (func (result i32)))"
                " (func $b (type 0) i32.const 7))");
            WasmModule module = module_parse(source);
            AssertEq(module.funcs.size(), 2U);
            ExpectEq(module.funcs[1].typeidx, 0U);
            Assert(module.types.size() >= 1U);
            Assert(module.types[0].is_func());
            Expect((module.types[0].func().results == std::vector<ValueType>{ValueType::i32}));
            Expect(module.types[0].func().params.empty());
            Expect(!module_validate(module).has_value());
        })
    })
    Category("a type id may be used before its definition", {
        Test("in a function import", {
            std::istringstream source("(module (import \"m\" \"f\" (func (type $t))) (type $t (func (param i32))))");
            WasmModule module = module_parse(source);
            AssertEq(module.imports.size(), 1U);
            Assert(std::holds_alternative<index_t>(module.imports[0].desc));
            ExpectEq(std::get<index_t>(module.imports[0].desc), 0U);
        })
        Test("in a global's reference type", {
            std::istringstream source("(module (global (ref null $t) (ref.null $t)) (type $t (struct)))");
            WasmModule module = module_parse(source);
            AssertEq(module.globals.size(), 1U);
            Expect(module.globals[0].type.type.is_concrete());
            ExpectEq(module.globals[0].type.type.typeidx, 0U);
            Expect(!module_validate(module).has_value());
        })
        Test("in another type's field", {
            std::istringstream source(
                "(module (type $a (struct (field (ref null $b)))) (type $b (struct)))");
            WasmModule module = module_parse(source);
            AssertEq(module.types.size(), 2U);
            Assert(module.types[0].is_struct());
            const StructType& a = std::get<StructType>(module.types[0].comp);
            AssertEq(a.fields.size(), 1U);
            const ValueType& field = std::get<ValueType>(a.fields[0].storage);
            Expect(field.is_concrete());
            ExpectEq(field.typeidx, 1U);
        })
        Test("in an instruction", {
            std::istringstream source(
                "(module (func (result (ref $t)) struct.new $t) (type $t (struct)))");
            WasmModule module = module_parse(source);
            AssertEq(module.funcs.size(), 1U);
            const WasmInstr& instr = module.funcs[0].body[0];
            Assert(instr.opcode == Opcode::Struct_new);
            ExpectEq(std::get<WasmInstr::OneIdx>(instr.imm).index, 0U);
            Expect(!module_validate(module).has_value());
        })
        Test("an id that is never defined", {
            ExpectEq(parse_error("(module (func (type $nope)))"),
                     std::string("type id '$nope' not found"));
        })
    })
};
