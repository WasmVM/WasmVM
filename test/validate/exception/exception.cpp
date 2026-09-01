// Copyright 2026 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// Validation of the exception-handling instructions: try_table, throw and
// throw_ref, plus the tag section they depend on.
//
// Before try_table validation existed, the instruction fell into the
// validator's default no-op: no control frame was pushed while its `end`
// still popped one, so a perfectly valid module carrying try_table was
// rejected with "empty validate control stack" before it could ever run.
// The "accepts" cases below pin the fix; the "rejects" cases pin the new
// checks so they cannot silently degrade back into no-ops.
//
// Note on catch label indices: in this codebase they are branch targets
// from within the try_table body — label 0 is the try_table's own label.
// The parser resolves symbolic labels that way and the runtime unwinder
// branches that way, so the validator follows the same convention.

#include <harness.hpp>
#include <WasmVM.hpp>
#include <exception.hpp>

#include <optional>
#include <sstream>
#include <string>

using namespace WasmVM;
using namespace Testing;

namespace {

std::optional<Exception::Exception> validate(const std::string& wat) {
    std::istringstream source(wat);
    WasmModule module = module_parse(source);
    return module_validate(module);
}

// A module with one i32-parameter tag ($t0, tag index 0) and one void
// function whose body is `body`.
std::string with_tag(const std::string& body) {
    return "(module (type (func (param i32))) (tag $t0 (type 0)) (func " + body + "))";
}

} // namespace

Suite validate_exception {
    Category("try_table accepts", {
        Test("no catches", {
            Expect(!validate("(module (func try_table nop end))"));
        })
        Test("catch_all targeting its own void label", {
            Expect(!validate("(module (func try_table (catch_all 0) nop end))"));
        })
        Test("catch targeting a label matching the tag params", {
            Expect(!validate(with_tag(
                "try_table (result i32) (catch $t0 0) i32.const 7 end drop")));
        })
        Test("catch_ref targeting a label with tag params plus exnref", {
            Expect(!validate(with_tag(
                "try_table (result i32 exnref) (catch_ref $t0 0) unreachable end drop drop")));
        })
        Test("catch_all_ref targeting a label with exnref", {
            Expect(!validate(
                "(module (func try_table (result exnref) (catch_all_ref 0) unreachable end drop))"));
        })
        Test("frame balance: try_table around a loop", {
            // The exact shape that the missing control frame used to break.
            Expect(!validate("(module (func try_table (catch_all 0) loop nop end end))"));
        })
    })
    Category("try_table rejects", {
        Test("catch label index out of range", {
            Expect(validate("(module (func try_table (catch_all 5) nop end))").has_value());
        })
        Test("catch tag index out of range", {
            Expect(validate("(module (func try_table (catch 3 0) nop end))").has_value());
        })
        Test("catch label type mismatch", {
            // Tag carries an i32 payload but the target label is void.
            Expect(validate(with_tag("try_table (catch $t0 0) nop end")).has_value());
        })
        Test("catch_all_ref label missing exnref", {
            Expect(validate("(module (func try_table (catch_all_ref 0) nop end))").has_value());
        })
    })
    Category("throw", {
        Test("accepts operands matching the tag params", {
            Expect(!validate(with_tag("i32.const 1 throw $t0")));
        })
        Test("rejects missing operands", {
            Expect(validate(with_tag("throw $t0")).has_value());
        })
        Test("rejects tag index out of range", {
            Expect(validate("(module (func throw 0))").has_value());
        })
    })
    Category("throw_ref", {
        Test("accepts an exnref operand", {
            Expect(!validate("(module (func (param exnref) local.get 0 throw_ref))"));
        })
        Test("rejects a non-exnref operand", {
            Expect(validate("(module (func i32.const 0 throw_ref))").has_value());
        })
    })
    Category("tag section", {
        Test("rejects a tag whose type index does not exist", {
            Expect(validate("(module (tag))").has_value());
        })
        Test("rejects a tag whose type has results", {
            Expect(validate("(module (type (func (result i32))) (tag (type 0)))").has_value());
        })
    })
};
