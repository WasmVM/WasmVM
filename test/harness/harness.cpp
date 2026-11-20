// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include "harness.hpp"

#include <fstream>

using namespace Testing;

std::optional<Suite>& Suite::registered(){
    static std::optional<Suite> suite;
    return suite;
}

Suite::Suite(std::initializer_list<SuiteItem> items) :
    items(items), total(0), failed(0)
{
    for(SuiteItem item : items){
        std::visit(overloaded {
            [&](TestType){
                total += 1;
            },
            [&](CategoryType category){
                total += category.total;
            }
        }, item);
    }
    registered().emplace(*this);
}

CategoryType::CategoryType(std::string name, std::initializer_list<SuiteItem> items) : 
    items(items), name(name), total(0), failed(0)
{
    for(SuiteItem item : items){
        std::visit(overloaded {
            [&](TestType){
                total += 1;
            },
            [&](CategoryType category){
                total += category.total;
            }
        }, item);
    }
}

TestType::TestType(std::string name, TestFuncType func) :
    name(name), func(func), passed(true)
{}