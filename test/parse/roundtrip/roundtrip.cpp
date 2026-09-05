// Copyright 2024 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <harness.hpp>
#include <WasmVM.hpp>
#include <exception.hpp>

#include <sstream>
#include <string>

using namespace WasmVM;
using namespace Testing;

static WasmModule roundtrip(const std::string& src) {
    std::istringstream in(src);
    WasmModule m1 = module_parse(in);
    std::ostringstream out;
    module_dump(m1, out);
    std::istringstream in2(out.str());
    return module_parse(in2);
}

// Text -> binary -> module: what wasmvm-as writes must be readable again.
static WasmModule binary_roundtrip(const std::string& src) {
    std::istringstream in(src);
    WasmModule m1 = module_parse(in);
    std::stringstream bin(std::ios::in | std::ios::out | std::ios::binary);
    module_encode(m1, bin);
    return module_decode(bin);
}

Suite roundtrip_suite {
    Test("empty module", {
        WasmModule m = roundtrip("(module)");
        Expect(m.types.empty());
        Expect(m.funcs.empty());
        Expect(m.imports.empty());
        Expect(m.exports.empty());
    })
    Test("function type", {
        WasmModule m = roundtrip("(module (type (func (param i32) (result i64))))");
        AssertEq(m.types.size(), 1U);
        AssertEq(m.types[0].params.size(), 1U);
        Expect(m.types[0].params[0] == ValueType::i32);
        AssertEq(m.types[0].results.size(), 1U);
        Expect(m.types[0].results[0] == ValueType::i64);
    })
    Test("function type deduplication preserved", {
        WasmModule m = roundtrip(
            "(module"
            "  (type (func (param i32)))"
            "  (type (func (param i32)))"
            ")");
        AssertEq(m.types.size(), 1U);
    })
    // exnref (0x69): the encoder already emitted it, the decoder used to
    // reject it as an invalid valtype (issue #3).
    Test("exnref function type survives binary roundtrip", {
        WasmModule m = binary_roundtrip("(module (type (func (param exnref) (result exnref))))");
        AssertEq(m.types.size(), 1U);
        AssertEq(m.types[0].params.size(), 1U);
        Expect(m.types[0].params[0] == ValueType::exnref);
        AssertEq(m.types[0].results.size(), 1U);
        Expect(m.types[0].results[0] == ValueType::exnref);
    })
    Test("exnref local survives binary roundtrip", {
        WasmModule m = binary_roundtrip("(module (func (local exnref)))");
        AssertEq(m.funcs.size(), 1U);
        AssertEq(m.funcs[0].locals.size(), 1U);
        Expect(m.funcs[0].locals[0] == ValueType::exnref);
    })
    Test("exnref local survives text roundtrip", {
        WasmModule m = roundtrip("(module (func (local exnref)))");
        AssertEq(m.funcs.size(), 1U);
        AssertEq(m.funcs[0].locals.size(), 1U);
        Expect(m.funcs[0].locals[0] == ValueType::exnref);
    })
    Test("memory limits", {
        WasmModule m = roundtrip("(module (memory 2 8))");
        AssertEq(m.mems.size(), 1U);
        ExpectEq(m.mems[0].min, 2U);
        Expect(m.mems[0].max.has_value());
        ExpectEq(m.mems[0].max.value(), 8U);
    })
    Test("table", {
        WasmModule m = roundtrip("(module (table 4 funcref))");
        AssertEq(m.tables.size(), 1U);
        ExpectEq(m.tables[0].limits.min, 4U);
        Expect(m.tables[0].reftype == RefType::funcref);
    })
    Test("global constant", {
        WasmModule m = roundtrip("(module (global i32 (i32.const 42)))");
        AssertEq(m.globals.size(), 1U);
        Expect(m.globals[0].type.type == ValueType::i32);
        Expect(m.globals[0].type.mut == GlobalType::constant);
        Expect(std::holds_alternative<Instr::I32_const>(m.globals[0].init));
        ExpectEq(std::get<Instr::I32_const>(m.globals[0].init).value, 42);
    })
    Test("export", {
        WasmModule m = roundtrip("(module (func) (export \"fn\" (func 0)))");
        AssertEq(m.exports.size(), 1U);
        Expect(m.exports[0].name == "fn");
        Expect(m.exports[0].desc == WasmExport::DescType::func);
        ExpectEq(m.exports[0].index, 0U);
    })
    Test("import", {
        WasmModule m = roundtrip("(module (import \"env\" \"fn\" (func (param i32))))");
        AssertEq(m.imports.size(), 1U);
        Expect(m.imports[0].module == "env");
        Expect(m.imports[0].name == "fn");
        Expect(std::holds_alternative<index_t>(m.imports[0].desc));
    })
    Test("data segment passive", {
        WasmModule m = roundtrip("(module (data \"hello\"))");
        AssertEq(m.datas.size(), 1U);
        Expect(m.datas[0].mode.type == WasmData::DataMode::Mode::passive);
    })
    Test("element segment passive", {
        WasmModule m = roundtrip("(module (func) (elem func 0))");
        AssertEq(m.elems.size(), 1U);
        Expect(m.elems[0].mode.type == WasmElem::ElemMode::Mode::passive);
    })
    // The tag section (0x0d): the parser filled module.tags and the decoder
    // could read the section, but the encoder never wrote one, so a module
    // with a tag came back with no tags and its `throw` named an index the
    // module did not define (issue #10).
    Test("tag survives binary roundtrip", {
        WasmModule m = binary_roundtrip("(module (tag (param i32)))");
        AssertEq(m.tags.size(), 1U);
        Assert(m.tags[0] < m.types.size());
        AssertEq(m.types[m.tags[0]].params.size(), 1U);
        Expect(m.types[m.tags[0]].params[0] == ValueType::i32);
        Expect(m.types[m.tags[0]].results.empty());
    })
    Test("several tags keep their order", {
        WasmModule m = binary_roundtrip(
            "(module (tag (param i32)) (tag) (tag (param i64)))");
        AssertEq(m.tags.size(), 3U);
        AssertEq(m.types[m.tags[0]].params.size(), 1U);
        Expect(m.types[m.tags[0]].params[0] == ValueType::i32);
        Expect(m.types[m.tags[1]].params.empty());
        AssertEq(m.types[m.tags[2]].params.size(), 1U);
        Expect(m.types[m.tags[2]].params[0] == ValueType::i64);
    })
    Test("a module that throws still validates after a binary roundtrip", {
        // The issue's reproduction: wasmvm-as wrote this module, and it
        // could not be loaded back because the tag index had no tag.
        WasmModule m = binary_roundtrip(
            "(module (tag $oops (param i32)) "
            "(func (export \"f\") (result i32) i32.const 7 throw $oops))");
        AssertEq(m.tags.size(), 1U);
        Expect(!module_validate(m).has_value());
    })
    Test("a module with no tag emits no tag section", {
        WasmModule m = binary_roundtrip("(module (func))");
        Expect(m.tags.empty());
        AssertEq(m.funcs.size(), 1U);
    })
};
