// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <iostream>
#include <string>
#include <vector>
#include <exception>
#include <sstream>
#include <regex>

#include "harness.hpp"

#ifdef CONSOLE_COLOR
#define COLOR_Warning "\033[1;33m[Warning]\033[0m"
#define COLOR_Pass "\033[32m[Pass]\033[0m"
#define COLOR_Fail "\033[31m[Failed]\033[0m"
#define COLOR_Fault "\033[1;35m[Fault]\033[0m"
#define COLOR_Note "\033[1;36m[Note]\033[0m"
#else
#define COLOR_Warning "! Warning"
#define COLOR_Pass "[Pass]"
#define COLOR_Fail "[Failed]"
#define COLOR_Fault "### Fault ###"
#define COLOR_Note "[Note]"
#endif

using namespace Testing;

struct Runner {

    Runner(size_t indent, size_t& failed) : indent(indent), failed(failed){}

    void operator()(Testing::TestType test){
        std::cout << std::string(indent + 1, ' ') <<  "- Run test: " << test.name << " ... ";
        std::stringstream stream;
        std::streambuf *coutbuf = std::cout.rdbuf();
        try{
            std::cout.rdbuf(stream.rdbuf());
            test.func(test.passed);
            std::cout.rdbuf(coutbuf);
        }catch(...){
            std::cout << COLOR_Fault " uncaught exception" << std::endl;
            failed += 1;
            return;
        }
        if(test.passed){
            std::cout << COLOR_Pass << std::endl;
        }else{
            std::cout << COLOR_Fail << std::endl;
            failed += 1;
        }
        std::string output = stream.str();
        if(!output.empty()){
            std::cout << std::string(indent + 3, ' ') << "stdout:" << std::endl;
            std::cout << std::string(indent + 3, ' ');
            std::regex_replace(std::ostreambuf_iterator<char>(std::cout), output.begin(), output.end(), std::regex("\n"), std::string("\n") + std::string(indent + 3, ' '));
            std::cout << std::endl;
        }
    }
    void operator()(Testing::CategoryType category){
        std::cout << std::endl << std::string(indent + 1, ' ') << "* " << category.name << " :" << std::endl;
        for(SuiteItem item : category.items){
            std::visit(Runner(indent + 2, category.failed), item);
        }
        std::cout << std::endl << std::string(indent + 3, ' ') << category.name << " : " << (category.total - category.failed) << " tests passed, " << category.failed << " tests failed out of "<< category.total << std::endl << std::endl;
        failed += category.failed;
    }
    size_t indent;
    size_t& failed;
};

int main(void){
    if(Suite::suite.has_value() == false){
        std::cerr << "=== " COLOR_Warning " Empty testsuite ===" << std::endl;
        return 0;
    }

    Suite& suite = *Suite::suite;

    for(SuiteItem item : suite.items){
        std::visit(Runner(0, suite.failed), item);
    }

    std::cout << std::endl << (suite.total - suite.failed) * 100 / suite.total << "% tests passed, "
        << suite.failed << " tests failed out of " << suite.total << std::endl << std::endl;
    return suite.failed;
}