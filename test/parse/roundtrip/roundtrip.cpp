// Copyright 2024 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <harness.hpp>
#include <WasmVM.hpp>
#include <exception.hpp>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <optional>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

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
//
// A module that validated before the roundtrip has to still validate after.
// The encoder losing a section or scrambling an immediate does not fail at
// the point it happens -- module_encode reports nothing and module_decode
// reads the damaged bytes back quite happily. It surfaces one step further
// on, as a module that no longer describes what it used to (issues #10, #14,
// #17). The check is conditional because the helper is also used on sources
// that do not validate in the first place, where "still invalid" says nothing.
static WasmModule binary_roundtrip(const std::string& src) {
    std::istringstream in(src);
    WasmModule m1 = module_parse(in);
    const bool valid_before = !module_validate(m1).has_value();
    std::stringstream bin(std::ios::in | std::ios::out | std::ios::binary);
    module_encode(m1, bin);
    WasmModule m2 = module_decode(bin);
    if (valid_before) {
        if (std::optional<Exception::Exception> error = module_validate(m2)) {
            throw Exception::Exception(
                std::string("valid before the binary roundtrip, invalid after: ") + error->what());
        }
    }
    return m2;
}

// The text of a module, as module_dump writes it.
static std::string text_of(WasmModule& module) {
    std::ostringstream out;
    module_dump(module, out);
    return out.str();
}

// Where the parse suite keeps one .wat per instruction family. Both suites run
// with their own source directory as the working directory.
static const char* instruction_fixtures = "../instruction";

// Fixtures the encoder is known not to read back yet, each with the issue that
// tracks it. The sweep below holds these to *failing*: when one of them starts
// reading back, the test goes red and says which entry to remove, so a fix
// cannot land without also retiring its exemption.
static const std::vector<std::string> known_not_reading_back = { // sorted: set_difference below
    "gc_array.wat",   // #19: one-index GC ops written with the 0xfc prefix
    "gc_struct.wat",  // #19
};

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
    // Tag export / import descriptors, both 0x04 (issue #14).
    Test("exported tag survives binary roundtrip", {
        WasmModule m = binary_roundtrip(
            "(module (tag $t (param i32)) (export \"t\" (tag $t)))");
        AssertEq(m.exports.size(), 1U);
        Expect(m.exports[0].name == "t");
        Expect(m.exports[0].desc == WasmExport::DescType::tag);
        ExpectEq(m.exports[0].index, 0U);
        Expect(!module_validate(m).has_value());
    })
    Test("imported tag survives binary roundtrip", {
        WasmModule m = binary_roundtrip(
            "(module (import \"env\" \"t\" (tag (param i32))))");
        AssertEq(m.imports.size(), 1U);
        Expect(m.imports[0].module == "env");
        Expect(m.imports[0].name == "t");
        Assert(std::holds_alternative<TagType>(m.imports[0].desc));
        index_t typeidx = std::get<TagType>(m.imports[0].desc).typeidx;
        Assert(typeidx < m.types.size());
        AssertEq(m.types[typeidx].params.size(), 1U);
        Expect(m.types[typeidx].params[0] == ValueType::i32);
        Expect(m.types[typeidx].results.empty());
        // An imported tag is not an imported function, even though both
        // descriptors carry a typeidx.
        Expect(!std::holds_alternative<index_t>(m.imports[0].desc));
    })
    Test("an imported tag takes the low index even when declared later", {
        // $own is written first but is module-defined, so the import still
        // owns tag index 0 and `throw $own` must resolve to 1.
        WasmModule m = binary_roundtrip(
            "(module"
            "  (tag $own (param i64))"
            "  (import \"env\" \"t\" (tag $imported (param i32)))"
            "  (export \"e1\" (tag $imported))"
            "  (export \"e2\" (tag $own)))");
        AssertEq(m.imports.size(), 1U);
        AssertEq(m.tags.size(), 1U);
        AssertEq(m.exports.size(), 2U);
        ExpectEq(m.exports[0].index, 0U);
        ExpectEq(m.exports[1].index, 1U);
        // The module-defined tag keeps its own type, the i64 one.
        AssertEq(m.types[m.tags[0]].params.size(), 1U);
        Expect(m.types[m.tags[0]].params[0] == ValueType::i64);
        Expect(!module_validate(m).has_value());
    })
    Test("a throw against an imported tag validates", {
        WasmModule m = binary_roundtrip(
            "(module (import \"env\" \"t\" (tag $t (param i32))) "
            "(func (export \"f\") i32.const 7 throw $t))");
        Expect(!module_validate(m).has_value());
    })
    Test("exporting a tag index that does not exist is rejected", {
        // The (func) is load-bearing: a module with no types at all fails
        // earlier, on the bare (tag) defaulting to type index 0, and this
        // case would then pass for the wrong reason.
        std::istringstream in("(module (func) (tag) (export \"t\" (tag 1)))");
        WasmModule m = module_parse(in);
        std::optional<Exception::Exception> err = module_validate(m);
        Assert(err.has_value());
        Expect(std::string(err->what()).find("tag index not found") != std::string::npos);
    })
    // The WAT dumper printed no (tag ...) while keeping the throw that named
    // one, so text round-tripping a tag-bearing module produced source that
    // could not be assembled again (issue #12).
    Test("tag survives the text roundtrip", {
        WasmModule m = roundtrip("(module (tag (param i32)))");
        AssertEq(m.tags.size(), 1U);
        Assert(m.tags[0] < m.types.size());
        AssertEq(m.types[m.tags[0]].params.size(), 1U);
        Expect(m.types[m.tags[0]].params[0] == ValueType::i32);
    })
    Test("a throwing module still validates after the text roundtrip", {
        WasmModule m = roundtrip(
            "(module (tag $oops (param i32)) "
            "(func (export \"f\") (result i32) i32.const 7 throw $oops))");
        AssertEq(m.tags.size(), 1U);
        Expect(!module_validate(m).has_value());
    })
    Test("text roundtrip keeps the tag index space with an import present", {
        // The import owns tag 0 and the module-defined tag owns 1. The dumper
        // prints the first in the import loop and the second in the tag loop,
        // so the two must not collide or swap.
        WasmModule m = roundtrip(
            "(module"
            "  (tag $own (param i64))"
            "  (import \"env\" \"t\" (tag $imported (param i32)))"
            "  (export \"e1\" (tag $imported))"
            "  (export \"e2\" (tag $own)))");
        AssertEq(m.imports.size(), 1U);
        AssertEq(m.tags.size(), 1U);
        AssertEq(m.exports.size(), 2U);
        ExpectEq(m.exports[0].index, 0U);
        ExpectEq(m.exports[1].index, 1U);
        AssertEq(m.types[m.tags[0]].params.size(), 1U);
        Expect(m.types[m.tags[0]].params[0] == ValueType::i64);
        Expect(!module_validate(m).has_value());
    })
    // An omitted typeuse is the empty type. It used to fall back to type
    // index 0, which is a real index, so a bare (tag) silently adopted
    // whatever type happened to sit there (issue #15).
    Test("a bare tag gets the empty type, not type 0", {
        WasmModule m = binary_roundtrip(
            "(module (type (func (param i32))) (tag))");
        AssertEq(m.tags.size(), 1U);
        Assert(m.tags[0] < m.types.size());
        Expect(m.types[m.tags[0]].params.empty());
        Expect(m.types[m.tags[0]].results.empty());
    })
    Test("a bare imported tag gets the empty type too", {
        WasmModule m = binary_roundtrip(
            "(module (type (func (param i32))) (import \"env\" \"t\" (tag)))");
        AssertEq(m.imports.size(), 1U);
        Assert(std::holds_alternative<TagType>(m.imports[0].desc));
        index_t typeidx = std::get<TagType>(m.imports[0].desc).typeidx;
        Assert(typeidx < m.types.size());
        Expect(m.types[typeidx].params.empty());
        Expect(m.types[typeidx].results.empty());
    })
    Test("a bare tag works in a module that declares no types at all", {
        WasmModule m = binary_roundtrip("(module (tag))");
        AssertEq(m.tags.size(), 1U);
        Assert(m.tags[0] < m.types.size());
        Expect(m.types[m.tags[0]].params.empty());
        Expect(!module_validate(m).has_value());
    })
    Test("throwing a bare tag needs nothing on the stack", {
        // The same source used to fail with "empty validate value stack",
        // because the tag had taken the declared (param i32) type.
        std::istringstream in(
            "(module (type (func (param i32))) (tag) "
            "(func (export \"f\") throw 0))");
        WasmModule m = module_parse(in);
        Expect(!module_validate(m).has_value());
    })
    Test("a catch on a bare tag sends nothing to its label", {
        // With the tag correctly empty, a label expecting an i32 no longer
        // matches. Before the fix the tag carried an i32 and this passed.
        std::istringstream in(
            "(module (type (func (param i32))) (tag) "
            "(func (export \"f\") (result i32) "
            "block $h (result i32) try_table (catch 0 $h) "
            "i32.const 7 throw 0 end unreachable end))");
        bool refused = false;
        try {
            WasmModule m = module_parse(in);
            refused = module_validate(m).has_value();
        } catch (Exception::Exception&) {
            refused = true;
        }
        Expect(refused);
    })
    Test("exporting a tag index that does exist is accepted", {
        std::istringstream in("(module (func) (tag) (export \"t\" (tag 0)))");
        WasmModule m = module_parse(in);
        Expect(!module_validate(m).has_value());
    })
    // memory.init carries memidx and dataidx, and the wire order (dataidx
    // first) is the reverse of the flat order (a = memidx, b = dataidx).
    // The flat-to-typed hop in the encoder swapped them a second time, so a
    // module written by wasmvm-as named a memory that did not exist and
    // could not be read back (issue #17).
    Test("memory.init keeps its memidx and dataidx across a binary roundtrip", {
        // Two data segments, one memory: a swap turns dataidx 1 into an
        // out-of-range memidx, which validation rejects.
        WasmModule m = binary_roundtrip(
            "(module (memory 1) (data \"AAAA\") (data \"BBBB\")"
            " (func (export \"f\") i32.const 0 i32.const 0 i32.const 4"
            " memory.init 1))");
        Expect(!module_validate(m).has_value());
        AssertEq(m.funcs.size(), 1U);
        auto found = std::find_if(m.funcs[0].body.begin(), m.funcs[0].body.end(),
            [](const WasmInstr& i){ return i.opcode == Opcode::Memory_init; });
        Assert(found != m.funcs[0].body.end());
        auto& imm = std::get<WasmInstr::TwoIdx>(found->imm);
        ExpectEq(imm.a, 0U); // memidx
        ExpectEq(imm.b, 1U); // dataidx
    })
    // A typed select is the untyped one plus a valtype vector, and the
    // flat-to-typed hop in the encoder built the typed form without copying
    // that vector, so every `select (result t)` was written as a plain
    // `select`. On a numeric type nothing shows; on a reference type the
    // module that wasmvm-as wrote no longer validates (issue #18).
    Test("a typed select keeps its valtypes across a binary roundtrip", {
        WasmModule m = binary_roundtrip(
            "(module (func (export \"f\") (param externref externref i32) (result externref)"
            " local.get 0 local.get 1 local.get 2 select (result externref)))");
        Expect(!module_validate(m).has_value());
        AssertEq(m.funcs.size(), 1U);
        auto found = std::find_if(m.funcs[0].body.begin(), m.funcs[0].body.end(),
            [](const WasmInstr& i){ return i.opcode == Opcode::Select; });
        Assert(found != m.funcs[0].body.end());
        auto& valtypes = std::get<WasmInstr::SelectV>(found->imm).valtypes;
        AssertEq(valtypes.size(), 1U);
        Expect(valtypes[0] == ValueType::externref);
    })
    // The parse suite keeps one .wat per instruction family, and between them
    // they name very nearly every opcode. Reading them back out of the binary
    // is the cheapest wide check there is on the encoder: three of the four
    // encoder bugs found so far (#10, #14, #17) were one instruction or one
    // section going quietly missing, and each of them was in a family this
    // directory already covers.
    //
    // module_dump is the comparison, not module_validate: most of these
    // fixtures are lists of opcodes with no regard for stack discipline, so
    // they do not validate before the roundtrip either and validation has
    // nothing to say about them. The text does -- it names every immediate,
    // so an index that comes back swapped or a valtype that comes back empty
    // is a different string.
    Test("every instruction fixture reads back as what was written", {
        std::vector<std::string> checked;
        std::vector<std::string> differed;
        for (const std::filesystem::directory_entry& entry :
                 std::filesystem::directory_iterator(instruction_fixtures)) {
            if (entry.path().extension() != ".wat") {
                continue;
            }
            const std::string name = entry.path().filename().string();
            checked.push_back(name);

            std::ifstream source(entry.path());
            WasmModule before = module_parse(source);
            std::stringstream binary(std::ios::in | std::ios::out | std::ios::binary);
            module_encode(before, binary);
            WasmModule after = module_decode(binary);

            if (text_of(before) != text_of(after)) {
                differed.push_back(name);
            }
        }
        std::sort(checked.begin(), checked.end());
        std::sort(differed.begin(), differed.end());

        // An empty or moved directory would make the loop above vacuous, and a
        // check that quietly does not run reads exactly like a passing one.
        Assert(!checked.empty());

        // Every known entry must still be a real fixture, or it exempts nothing.
        for (const std::string& name : known_not_reading_back) {
            Assert(std::binary_search(checked.begin(), checked.end(), name));
        }

        std::vector<std::string> unexpected_failures;
        std::set_difference(differed.begin(), differed.end(),
                            known_not_reading_back.begin(), known_not_reading_back.end(),
                            std::back_inserter(unexpected_failures));
        std::vector<std::string> now_reading_back;
        std::set_difference(known_not_reading_back.begin(), known_not_reading_back.end(),
                            differed.begin(), differed.end(),
                            std::back_inserter(now_reading_back));

        for (const std::string& name : unexpected_failures) {
            std::cout << "      " << name << " does not read back as written" << std::endl;
        }
        for (const std::string& name : now_reading_back) {
            std::cout << "      " << name << " now reads back: remove it from known_not_reading_back"
                      << std::endl;
        }
        ExpectEq(unexpected_failures.size(), 0U);
        ExpectEq(now_reading_back.size(), 0U);
    })
};
