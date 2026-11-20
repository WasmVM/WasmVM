// Copyright 2026 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// Nothing may follow the `end` that closes a function (issue #49).
//
// That `end` pops the function's own control frame. The validator used to
// carry on with the next instruction anyway: `drop` then read the top of the
// empty control stack and crashed the host, `i32.const 0` never looked at the
// control stack and passed as valid, and only a second `end` was refused.
// The decoder accepts such a body from a binary, so a 25-byte file was
// enough to crash wasmvm and readwasm.

#include <harness.hpp>
#include <WasmVM.hpp>
#include <exception.hpp>

#include <optional>
#include <sstream>
#include <string>
#include <vector>

using namespace WasmVM;
using namespace Testing;

namespace {

// `(module (func))`, whose body is just the closing `end`, with `extra`
// appended after it.
WasmModule after_end(const WasmInstr& extra) {
    std::istringstream source("(module (func))");
    WasmModule module = module_parse(source);
    module.funcs.at(0).body.push_back(extra);
    return module;
}

// Rejected at the instruction after the closing `end` (the second one).
bool rejected_after_end(const WasmModule& module) {
    std::optional<Exception::Exception> error = module_validate(module);
    if (!error) {
        std::cout << "      accepted, expected rejection at func[0]:2" << std::endl;
        return false;
    }
    if (std::string(error->what()).find("func[0]:2:") == std::string::npos) {
        std::cout << "      rejected elsewhere: " << error->what() << std::endl;
        return false;
    }
    return true;
}

WasmModule decode(const std::string& bytes) {
    std::istringstream binary(bytes, std::ios::in | std::ios::binary);
    return module_decode(binary);
}

// One function of type [] -> []; `body` is the code entry after its size byte.
std::string module_with_body(const std::string& body) {
    const std::string header("\x00\x61\x73\x6d\x01\x00\x00\x00"   // magic, version
                             "\x01\x04\x01\x60\x00\x00"           // type section: [] -> []
                             "\x03\x02\x01\x00", 18);              // func section: type 0
    std::string code;
    code += '\x01';                                                // one entry
    code += static_cast<char>(body.size());
    code += body;
    return header + '\x0a' + static_cast<char>(code.size()) + code;
}

const std::string end_only("\x00\x0b", 2);        // no locals, end
const std::string end_drop("\x00\x0b\x1a", 3);    // no locals, end, drop

} // namespace

Suite validate_function_end {
    Category("through the API", {
        Test("a body that is just its closing end is accepted", {
            std::istringstream source("(module (func))");
            WasmModule module = module_parse(source);
            AssertEq(module.funcs.at(0).body.size(), 1U);
            Expect(!module_validate(module));
        })
        Test("end end is refused", {
            Expect(rejected_after_end(after_end(WasmInstr(Instr::End()))));
        })
        Test("end i32.const 0 is refused", {
            // Used to be accepted.
            Expect(rejected_after_end(after_end(WasmInstr(Instr::I32_const(0)))));
        })
        Test("end drop is refused", {
            // Used to crash the host.
            Expect(rejected_after_end(after_end(WasmInstr(Instr::Drop()))));
        })
    })
    Category("from a binary", {
        Test("the well-formed module decodes and is accepted", {
            const std::string bytes = module_with_body(end_only);
            AssertEq(bytes.size(), 24U);
            WasmModule module = decode(bytes);
            AssertEq(module.funcs.at(0).body.size(), 1U);
            Expect(!module_validate(module));
        })
        Test("the 25-byte end drop module is refused", {
            // Used to crash wasmvm and readwasm.
            const std::string bytes = module_with_body(end_drop);
            AssertEq(bytes.size(), 25U);
            WasmModule module = decode(bytes);
            AssertEq(module.funcs.at(0).body.size(), 2U);
            Expect(rejected_after_end(module));
        })
    })
};
