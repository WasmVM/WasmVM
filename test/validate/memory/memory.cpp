// Copyright 2026 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// A memory type's declared limits.
//
// The spec validates a memory type's limits within 2^(|addrtype|-16): 2^16
// pages for a 32-bit memory, 2^48 for a 64-bit one, each inclusive. The
// validator used 2^48 for both, so a 32-bit memory could declare up to 2^48
// pages and pass, though the runtime grow bound (invoke/memory.cpp) caps a
// 32-bit memory at 2^16 and instantiating such a min would try to allocate
// gigabytes to terabytes. The bound now follows the memory's index type.

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

bool accepted(const std::string& wat) {
    std::optional<Exception::Exception> error = validate(wat);
    if (error) {
        std::cout << "      rejected: " << error->what() << std::endl;
        return false;
    }
    return true;
}

bool rejected_with(const std::string& wat, const std::string& fragment) {
    std::optional<Exception::Exception> error;
    try {
        error = validate(wat);
    } catch (Exception::Parse& e) {
        std::cout << "      parse refused: " << e.what() << std::endl;
        return false;
    }
    if (!error) {
        std::cout << "      accepted, expected rejection mentioning '" << fragment << "'" << std::endl;
        return false;
    }
    if (std::string(error->what()).find(fragment) == std::string::npos) {
        std::cout << "      rejected for another reason: " << error->what() << std::endl;
        return false;
    }
    return true;
}

} // namespace

Suite validate_memory {
    Category("a 32-bit memory is limited to 2^16 pages", {
        Test("a small memory is accepted", {
            Expect(accepted("(module (memory 1))"));
        })
        Test("exactly 2^16 pages is accepted", {
            Expect(accepted("(module (memory 65536))"));
        })
        Test("more than 2^16 pages is rejected", {
            Expect(rejected_with("(module (memory 65537))", "memory size too large"));
        })
        Test("a max over 2^16 pages is rejected", {
            Expect(rejected_with("(module (memory 1 65537))", "memory size too large"));
        })
    })
    Category("a 64-bit memory is limited to 2^48 pages", {
        Test("more than 2^16 pages is accepted for a 64-bit memory", {
            Expect(accepted("(module (memory i64 65537))"));
        })
        Test("exactly 2^48 pages is accepted", {
            Expect(accepted("(module (memory i64 281474976710656))"));
        })
        Test("more than 2^48 pages is rejected", {
            Expect(rejected_with("(module (memory i64 281474976710657))", "memory size too large"));
        })
    })
};
