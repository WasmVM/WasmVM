// Copyright 2026 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// Linking modules that carry tags.
//
// wasmvm-ld had no tag index space at all: it dropped the tag section while
// keeping the `throw` that named one, so the linked module could not be
// loaded and the linker still exited 0 (issue #13). It also never remapped
// the tag index in `throw` or in a try_table catch clause, so once more than
// one module contributed tags the indices had to move.

#include <harness.hpp>
#include <WasmVM.hpp>
#include <exception.hpp>
#include <Linker.hpp>

#include <sstream>
#include <string>
#include <vector>

using namespace WasmVM;
using namespace Testing;

namespace {

// Assemble WAT into the binary form the linker consumes.
std::string assemble(const std::string& wat) {
    std::istringstream in(wat);
    WasmModule module = module_parse(in);
    if (std::optional<Exception::Exception> error = module_validate(module)) {
        throw *error;
    }
    std::ostringstream out;
    module_encode(module, out);
    return out.str();
}

// Link the given (path, WAT) pairs and return the merged module.
WasmModule link(const std::vector<std::pair<std::string, std::string>>& inputs) {
    Linker::Config config;
    config.start_func = Linker::Config::StartMode::None;
    Linker linker(config);
    for (const auto& [path, wat] : inputs) {
        std::string binary = assemble(wat);
        std::istringstream stream(binary);
        linker.consume(path, stream);
    }
    return linker.get();
}

const std::string tag_a =
    "(module (tag $a (param i32)) (func (export \"fa\") i32.const 1 throw $a))";
const std::string tag_b =
    "(module (tag $b (param i64)) (func (export \"fb\") i64.const 2 throw $b))";

index_t throw_index(const WasmModule& m, size_t func) {
    for (const WasmInstr& instr : m.funcs[func].body) {
        if (instr.opcode == Opcode::Throw) {
            return std::get<WasmInstr::OneIdx>(instr.imm).index;
        }
    }
    throw Exception::Exception("no throw in that function");
}

} // namespace

Suite linker_suite {
    Category("tags survive linking", {
        Test("a single module keeps its tag", {
            WasmModule m = link({{"a.wasm", tag_a}});
            AssertEq(m.tags.size(), (size_t)1);
            AssertEq(m.types[m.tags[0]].func().params.size(), (size_t)1);
            Expect(m.types[m.tags[0]].func().params[0] == ValueType::i32);
            ExpectEq(throw_index(m, 0), (index_t)0);
        })
        Test("two modules keep both tags, with the second index shifted", {
            WasmModule m = link({{"a.wasm", tag_a}, {"b.wasm", tag_b}});
            AssertEq(m.tags.size(), (size_t)2);
            Expect(m.types[m.tags[0]].func().params[0] == ValueType::i32);
            Expect(m.types[m.tags[1]].func().params[0] == ValueType::i64);
            // Each module wrote `throw 0` against its own tag; only the
            // second one has to move.
            ExpectEq(throw_index(m, 0), (index_t)0);
            ExpectEq(throw_index(m, 1), (index_t)1);
        })
        Test("a try_table catch clause is remapped too", {
            const std::string with_catch =
                "(module (tag $c (param i32)) (func (export \"fc\") (result i32) "
                "block $h (result i32) try_table (catch $c $h) "
                "i32.const 5 throw $c end unreachable end))";
            WasmModule m = link({{"a.wasm", tag_a}, {"c.wasm", with_catch}});
            AssertEq(m.tags.size(), (size_t)2);
            bool seen = false;
            for (const WasmInstr& instr : m.funcs[1].body) {
                if (instr.opcode == Opcode::Try_table) {
                    auto& tt = std::get<WasmInstr::TryTab>(instr.imm);
                    AssertEq(tt.catches.size(), (size_t)1);
                    ExpectEq(tt.catches[0].tag_idx, (index_t)1);
                    seen = true;
                }
            }
            // The loop must have found the instruction, or the assertion
            // above never ran.
            Expect(seen);
            ExpectEq(throw_index(m, 1), (index_t)1);
        })
        Test("an imported tag resolves to the exporting module's tag", {
            const std::string exporter =
                "(module (tag $t (param i32)) (export \"t\" (tag $t)) "
                "(func (export \"fd\") i32.const 1 throw $t))";
            const std::string importer =
                "(module (import \"d.wasm\" \"t\" (tag $t (param i32))) "
                "(func (export \"fe\") i32.const 2 throw $t))";
            WasmModule m = link({{"d.wasm", exporter}, {"e.wasm", importer}});
            // One tag, shared: the import did not create a second one.
            AssertEq(m.tags.size(), (size_t)1);
            Expect(m.imports.empty());
            ExpectEq(throw_index(m, 0), (index_t)0);
            ExpectEq(throw_index(m, 1), (index_t)0);
        })
        Test("a module with no tag links to no tag section", {
            WasmModule m = link({{"n.wasm", "(module (func (export \"f\")))"}});
            Expect(m.tags.empty());
        })
    })
    // GC type definitions land after the types of the modules linked before
    // them, and every instruction naming one has to move with them.
    Category("GC types survive linking", {
        Test("a struct type and its struct.new shift together", {
            WasmModule m = link({
                {"a.wasm", "(module (type (func (param i32))) (func (export \"fa\") (param i32)))"},
                {"b.wasm", "(module (type $s (struct (field i32)))"
                           " (func (export \"fb\") i32.const 1 struct.new $s drop))"},
            });
            index_t named = index_npos;
            for (const WasmInstr& instr : m.funcs[1].body) {
                if (instr.opcode == Opcode::Struct_new) named = std::get<WasmInstr::OneIdx>(instr.imm).index;
            }
            Assert(named != index_npos);
            // The struct type is the only one in the output, and a's types
            // pushed it off index 0.
            index_t struct_idx = index_npos;
            for (index_t i = 0; i < m.types.size(); ++i) {
                if (m.types[i].is_struct()) { Assert(struct_idx == index_npos); struct_idx = i; }
            }
            Assert(struct_idx != index_npos);
            Expect(struct_idx != 0U);
            ExpectEq(named, struct_idx);
            // The linked module still describes what it meant.
            Expect(!module_validate(m).has_value());
        })
        Test("a struct type and the heap type naming it shift together", {
            WasmModule m = link({
                {"a.wasm", "(module (type (func (param i32))) (func (export \"fa\") (param i32)))"},
                // The operand of a cast comes from the top of the target's
                // hierarchy, so a struct test takes an anyref, not a funcref.
                {"b.wasm", "(module (type $s (struct (field i32)))"
                           " (func (export \"fb\") (param anyref) local.get 0 ref.test (ref $s) drop))"},
            });
            int32_t heaptype = -1;
            for (const WasmInstr& instr : m.funcs[1].body) {
                if (instr.opcode == Opcode::Ref_test) heaptype = std::get<WasmInstr::HeapI32>(instr.imm).heaptype;
            }
            // The struct type is the only one in the output, and it is no
            // longer at index 0: a's types come first.
            index_t struct_idx = index_npos;
            for (index_t i = 0; i < m.types.size(); ++i) {
                if (m.types[i].is_struct()) { Assert(struct_idx == index_npos); struct_idx = i; }
            }
            Assert(struct_idx != index_npos);
            Expect(struct_idx != 0U);
            ExpectEq(heaptype, (int32_t)struct_idx);
        })
        Test("a rec group keeps its bounds and its supertype after the shift", {
            WasmModule m = link({
                {"a.wasm", "(module (type (func)) (func (export \"fa\")))"},
                {"b.wasm", "(module (rec (type $p (sub (struct (field i32)))) (type $q (sub $p (struct (field i32) (field i64)))))"
                           " (func (export \"fb\")))"},
            });
            AssertEq(m.types.size(), 3U);
            AssertEq(m.recs.size(), 1U);
            ExpectEq(m.recs[0].first, 1U);
            ExpectEq(m.recs[0].count, 2U);
            // $q's supertype $p moved from 0 to 1 with the group.
            AssertEq(m.types[2].supers.size(), 1U);
            ExpectEq(m.types[2].supers[0], 1U);
        })
        Test("identical struct types from two modules stay distinct", {
            WasmModule m = link({
                {"a.wasm", "(module (type (struct (field i32))) (func (export \"fa\")))"},
                {"b.wasm", "(module (type (struct (field i32))) (func (export \"fb\")))"},
            });
            // One shared () -> () function type, two struct types.
            ExpectEq(m.types.size(), 3U);
        })
    })
};
