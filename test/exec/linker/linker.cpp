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
            AssertEq(m.types[m.tags[0]].params.size(), (size_t)1);
            Expect(m.types[m.tags[0]].params[0] == ValueType::i32);
            ExpectEq(throw_index(m, 0), (index_t)0);
        })
        Test("two modules keep both tags, with the second index shifted", {
            WasmModule m = link({{"a.wasm", tag_a}, {"b.wasm", tag_b}});
            AssertEq(m.tags.size(), (size_t)2);
            Expect(m.types[m.tags[0]].params[0] == ValueType::i32);
            Expect(m.types[m.tags[1]].params[0] == ValueType::i64);
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
};
