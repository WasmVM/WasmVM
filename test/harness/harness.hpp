#ifndef WASMVM_TEST_HARNESS_DEF
#define WASMVM_TEST_HARNESS_DEF

#include <initializer_list>
#include <list>
#include <variant>
#include <optional>
#include <functional>
#include <iostream>

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

    static std::optional<Suite> suite;

    std::list<SuiteItem> items;
    size_t total, failed;
};

}

#define Test(NAME, STMT) Testing::TestType(NAME, [](bool& _is_passed) STMT),

#define Category(NAME, TESTS) Testing::CategoryType(NAME, TESTS),

#define Expect(EXPR) if(!(EXPR)) { _is_passed = false; }

#define Assert(EXPR) if(!(EXPR)) { _is_passed = false; return;}

#endif