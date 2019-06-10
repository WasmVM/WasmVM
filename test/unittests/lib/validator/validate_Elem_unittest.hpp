#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <stdint.h>
#include <stdlib.h>
#include <dataTypes/Value.h>
#include <dataTypes/vector_p.h>
#include <structures/WasmElem.h>
#include <structures/WasmFunc.h>
#include <structures/WasmTable.h>
#include <structures/WasmModule.h>
#include <Validates.h>
}
#undef _Bool

SKYPAT_F(Validate_Elem, valid)
{
    // Prepare
    WasmElem* elem = new_WasmElem();
    WasmModule* module = new_WasmModule(NULL);
    WasmTable* table = (WasmTable*)malloc(sizeof(WasmTable));
    WasmFunc* func1 = new_WasmFunc();
    WasmFunc* func2 = new_WasmFunc();
    vector_push_back(module->tables, table);
    vector_push_back(module->funcs, func1);
    vector_push_back(module->funcs, func2);
    elem->offset.parent.entryType = Entry_Value;
    elem->offset.type = Value_i32;
    elem->offset.value.i32 = 0;
    uint32_t* index = (uint32_t*) malloc(sizeof(uint32_t));
    *index = 1;
    vector_push_back(elem->init, index);

    // Check
    EXPECT_EQ(validate_Elem(elem, module), 0);

    free_WasmElem(elem);
    free_WasmModule(module);
    free(index);
}

SKYPAT_F(Validate_Elem, func_index_out_of_range)
{
    // Prepare
    WasmElem* elem = new_WasmElem();
    WasmModule* module = new_WasmModule(NULL);
    WasmTable* table = (WasmTable*)malloc(sizeof(WasmTable));
    WasmFunc* func1 = new_WasmFunc();
    WasmFunc* func2 = new_WasmFunc();
    vector_push_back(module->tables, table);
    vector_push_back(module->funcs, func1);
    vector_push_back(module->funcs, func2);
    elem->offset.parent.entryType = Entry_Value;
    elem->offset.type = Value_i32;
    elem->offset.value.i32 = 0;
    uint32_t* index = (uint32_t*) malloc(sizeof(uint32_t));
    *index = 2;
    vector_push_back(elem->init, index);

    // Check
    EXPECT_NE(validate_Elem(elem, module), 0);

    free_WasmElem(elem);
    free_WasmModule(module);
    free(index);
}


SKYPAT_F(Validate_Elem, table_index_out_of_range)
{
    // Prepare
    WasmElem* elem = new_WasmElem();
    WasmModule* module = new_WasmModule(NULL);
    WasmTable* table = (WasmTable*)malloc(sizeof(WasmTable));
    WasmFunc* func1 = new_WasmFunc();
    WasmFunc* func2 = new_WasmFunc();
    vector_push_back(module->tables, table);
    vector_push_back(module->funcs, func1);
    vector_push_back(module->funcs, func2);
    elem->table = 1;
    elem->offset.parent.entryType = Entry_Value;
    elem->offset.type = Value_i32;
    elem->offset.value.i32 = 0;
    uint32_t* index = (uint32_t*) malloc(sizeof(uint32_t));
    *index = 0;
    vector_push_back(elem->init, index);

    // Check
    EXPECT_NE(validate_Elem(elem, module), 0);

    free_WasmElem(elem);
    free_WasmModule(module);
    free(index);
}

SKYPAT_F(Validate_Elem, expr_not_i32)
{
    // Prepare
    WasmElem* elem = new_WasmElem();
    WasmModule* module = new_WasmModule(NULL);
    WasmTable* table = (WasmTable*)malloc(sizeof(WasmTable));
    WasmFunc* func1 = new_WasmFunc();
    WasmFunc* func2 = new_WasmFunc();
    vector_push_back(module->tables, table);
    vector_push_back(module->funcs, func1);
    vector_push_back(module->funcs, func2);
    elem->offset.parent.entryType = Entry_Value;
    elem->offset.type = Value_f32;
    elem->offset.value.i32 = 0;
    uint32_t* index = (uint32_t*) malloc(sizeof(uint32_t));
    *index = 0;
    vector_push_back(elem->init, index);

    // Check
    EXPECT_NE(validate_Elem(elem, module), 0);

    free_WasmElem(elem);
    free_WasmModule(module);
    free(index);
}
