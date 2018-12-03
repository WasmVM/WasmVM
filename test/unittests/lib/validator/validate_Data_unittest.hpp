#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <stdint.h>
#include <stdlib.h>
#include <dataTypes/Value.h>
#include <structures/WasmData.h>
#include <structures/WasmMemory.h>
#include <structures/WasmModule.h>
#include <Validates.h>
}
#undef _Bool

SKYPAT_F(Validate_Data, valid)
{
    // Prepare
    WasmData* data = new_WasmData();
    WasmModule* module = new_WasmModule(NULL);
    module->mems->push_back(module->mems, malloc(sizeof(WasmMemory)));
    data->data = 0;
    data->offset.parent.entryType = Entry_Value;
    data->offset.type = Value_i32;
    data->offset.value.i32 = 0;
    data->init->push_back(data->init, malloc(sizeof(char) * 4));

    // Check
    EXPECT_EQ(validate_Data(data, module), 0);

    free_WasmData(data);
    free_WasmModule(module);
}

SKYPAT_F(Validate_Data, mem_index_out_of_range)
{
    // Prepare
    WasmData* data = new_WasmData();
    WasmModule* module = new_WasmModule(NULL);
    module->mems->push_back(module->mems, malloc(sizeof(WasmMemory)));
    data->data = 1;
    data->offset.parent.entryType = Entry_Value;
    data->offset.type = Value_i32;
    data->offset.value.i32 = 0;
    data->init->push_back(data->init, malloc(sizeof(char) * 4));

    // Check
    EXPECT_NE(validate_Data(data, module), 0);

    free_WasmData(data);
    free_WasmModule(module);
}

SKYPAT_F(Validate_Data, expr_not_i32)
{
    // Prepare
    WasmData* data = new_WasmData();
    WasmModule* module = new_WasmModule(NULL);
    module->mems->push_back(module->mems, malloc(sizeof(WasmMemory)));
    data->data = 0;
    data->offset.parent.entryType = Entry_Value;
    data->offset.type = Value_f32;
    data->offset.value.i32 = 0;
    data->init->push_back(data->init, malloc(sizeof(char) * 4));

    // Check
    EXPECT_NE(validate_Data(data, module), 0);

    free_WasmData(data);
    free_WasmModule(module);
}
