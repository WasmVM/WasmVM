// Copyright 2023 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include <harness.hpp>
#include <WasmVM.hpp>
#include <exception.hpp>
#include <string_view>

using namespace WasmVM;
using namespace Testing;

Suite elem {
    Test("passive", {
        ParseFile(test_module, "passive.wat");

        WasmData& data0 = test_module.datas[0];
        Expect(data0.mode.type == WasmData::DataMode::Mode::passive);
        Expect(std::string_view((const char*)data0.init.data(), 3) == "123");

        WasmData& data1 = test_module.datas[1];
        Expect(data1.mode.type == WasmData::DataMode::Mode::passive);
        Expect(std::string_view((const char*)data1.init.data(), 3) == "456");

        WasmData& data2 = test_module.datas[2];
        Expect(data2.mode.type == WasmData::DataMode::Mode::passive);
        Expect(std::string_view((const char*)data2.init.data(), 6) == "\t\n\r\'\\\"");

        WasmData& data3 = test_module.datas[3];
        Expect(data3.mode.type == WasmData::DataMode::Mode::passive);
        Expect(std::string_view((const char*)data3.init.data(), 2) == "\x34\x12");

        WasmData& data4 = test_module.datas[4];
        Expect(data4.mode.type == WasmData::DataMode::Mode::passive);
        Expect(data4.init[0] == byte_t{0x00});
        Expect(data4.init[1] == byte_t{0xe0});

        WasmData& data5 = test_module.datas[5];
        Expect(data5.mode.type == WasmData::DataMode::Mode::passive);
        Expect(data5.init[0] == byte_t{0x01});
        Expect(data5.init[1] == byte_t{0x12});
        Expect(data5.init[2] == byte_t{0x23});
        Expect(data5.init[3] == byte_t{0x34});
        Expect(data5.init[4] == byte_t{0x45});
        Expect(data5.init[5] == byte_t{0x56});
        Expect(data5.init[6] == byte_t{0x67});
        Expect(data5.init[7] == byte_t{0x78});
        Expect(data5.init[8] == byte_t{0x89});
        Expect(data5.init[9] == byte_t{0x9a});
        Expect(data5.init[10] == byte_t{0xab});
        Expect(data5.init[11] == byte_t{0xbc});
        Expect(data5.init[12] == byte_t{0xcd});
        Expect(data5.init[13] == byte_t{0xde});
        Expect(data5.init[14] == byte_t{0xef});
        Expect(data5.init[15] == byte_t{0xfA});
        Expect(data5.init[16] == byte_t{0x0B});
        Expect(data5.init[17] == byte_t{0x1C});
        Expect(data5.init[18] == byte_t{0x2D});
        Expect(data5.init[19] == byte_t{0x3E});
        Expect(data5.init[20] == byte_t{0x4F});
    })
    Test("active", {
        ParseFile(test_module, "active.wat");

        WasmData& data0 = test_module.datas[0];
        Expect(data0.mode.type == WasmData::DataMode::Mode::active);
        Expect(std::string_view((const char*)data0.init.data(), 2) == "12");

        WasmData& data1 = test_module.datas[1];
        Expect(data1.mode.type == WasmData::DataMode::Mode::active);
        Expect(std::string_view((const char*)data1.init.data(), 2) == "34");

        WasmData& data2 = test_module.datas[2];
        Expect(data2.mode.type == WasmData::DataMode::Mode::active);
        Expect(std::string_view((const char*)data2.init.data(), 2) == "56");

        WasmData& data3 = test_module.datas[3];
        Expect(data3.mode.type == WasmData::DataMode::Mode::active);
        Expect(std::string_view((const char*)data3.init.data(), 2) == "78");
    })
};