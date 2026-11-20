#ifndef WASMVM_TEST_HARNESS_DEF
#define WASMVM_TEST_HARNESS_DEF

#include <initializer_list>
#include <list>
#include <variant>
#include <optional>
#include <functional>
#include <iostream>
#include <fstream>
#include <filesystem>

#include <Util.hpp>
#include <WasmVM.hpp>

namespace Testing {

struct TestType;
struct CategoryType;

using SuiteItem = std::variant<CategoryType, TestType>;
using TestFuncType = std::function<void(bool&)>;

struct TestType {
    TestType(std::string name, TestFuncType func);
    std::string name;
    TestFuncType func;
    bool passed;
};

struct CategoryType {
    CategoryType(std::string name, std::initializer_list<SuiteItem> items);

    std::list<SuiteItem> items;
    std::string name;
    size_t total, failed;
}; 

struct Suite {
    
    Suite(std::initializer_list<SuiteItem> items);

    // The suite a test file registered. A function-local static, built on
    // first use: a namespace-scope static here would be initialized in
    // harness.cpp's own dynamic-init pass, which (MSVC Debug, /Od) can run
    // after the test file's Suite constructor and wipe the registration.
    static std::optional<Suite>& registered();

    std::list<SuiteItem> items;
    size_t total, failed;
};

}

#define Test(NAME, STMT) Testing::TestType(NAME, [](bool& _is_passed) { STMT }),

#define Category(NAME, TESTS) Testing::CategoryType(NAME, TESTS),

#define Expect(EXPR) if(!(EXPR)) { _is_passed = false; }

#define Assert(EXPR) if(!(EXPR)) { _is_passed = false; return;}

#define Throw(EXCEPTION, STMT) try { STMT; _is_passed = false;} catch( EXCEPTION ){ _is_passed = true; }

#define ExpectEq(ACTUAL, EXPECTED) \
    if (!((ACTUAL) == (EXPECTED))) { \
        std::cout << "Expected: " << (EXPECTED) << ", Got: " << (ACTUAL) << "\n"; \
        _is_passed = false; \
    }

#define AssertEq(ACTUAL, EXPECTED) \
    if (!((ACTUAL) == (EXPECTED))) { \
        std::cout << "Expected: " << (EXPECTED) << ", Got: " << (ACTUAL) << "\n"; \
        _is_passed = false; return; \
    }

#define ExpectThrows(EXCEPTION, STMT) \
    try { STMT; \
        std::cout << "Expected " #EXCEPTION " to be thrown\n"; \
        _is_passed = false; \
    } catch(EXCEPTION&) {}

#define ForEach(VAR, CONTAINER, STMT) \
    for (auto& VAR : CONTAINER) { STMT }

#define ParseFile(NAME, FILEPATH) \
    std::ifstream NAME ## _stream(FILEPATH); \
    WasmVM::WasmModule NAME = WasmVM::module_parse(NAME ## _stream)

#endif