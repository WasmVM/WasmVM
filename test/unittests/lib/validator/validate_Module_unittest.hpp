#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <dataTypes/Value.h>
#include <dataTypes/DescType.h>
#include <dataTypes/FuncType.h>
#include <structures/WasmFunc.h>
#include <structures/WasmGlobal.h>
#include <structures/WasmExport.h>
#include <structures/WasmModule.h>
#include <Validates.h>
}
#undef _Bool

SKYPAT_F(validate_Module, valid)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);

    FuncType type = new_FuncType();
    vector_push_back(module->types, type);
    module->start = 0;
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    vector_push_back(module->funcs, func);
    WasmGlobal* global = (WasmGlobal*) malloc(sizeof(WasmGlobal));
    vector_push_back(module->globals, global);
    WasmExport* export1 = (WasmExport*) malloc(sizeof(WasmExport));
    export1->name = (char*) malloc(sizeof(char) * 6);
    strcpy(export1->name, "test1");
    export1->descType = Desc_Func;
    export1->descIdx = 0;
    WasmExport* export2 = (WasmExport*) malloc(sizeof(WasmExport));
    export2->name = (char*) malloc(sizeof(char) * 6);
    strcpy(export2->name, "test2");
    export2->descType = Desc_Global;
    export2->descIdx = 0;

    // Check
    EXPECT_EQ(validate_Module(module), 0);

    free_WasmModule(module);
}

SKYPAT_F(validate_Module, start_function_not_exist)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);

    FuncType type = new_FuncType();
    vector_push_back(module->types, type);
    module->start = 1;
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    vector_push_back(module->funcs, func);
    WasmGlobal* global = (WasmGlobal*) malloc(sizeof(WasmGlobal));
    vector_push_back(module->globals, global);
    WasmExport* export1 = (WasmExport*) malloc(sizeof(WasmExport));
    export1->name = (char*) malloc(sizeof(char) * 6);
    strcpy(export1->name, "test1");
    export1->descType = Desc_Func;
    export1->descIdx = 0;
    vector_push_back(module->exports, export1);
    WasmExport* export2 = (WasmExport*) malloc(sizeof(WasmExport));
    export2->name = (char*) malloc(sizeof(char) * 6);
    strcpy(export2->name, "test2");
    export2->descType = Desc_Global;
    export2->descIdx = 0;
    vector_push_back(module->exports, export2);

    // Check
    EXPECT_EQ(validate_Module(module), -1);

    free_WasmModule(module);
}

SKYPAT_F(validate_Module, start_function_not_void)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);

    FuncType type = new_FuncType();
    vector_push_back(module->types, type);
    module->start = 0;
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    vector_push_back(module->funcs, func);
    WasmGlobal* global = (WasmGlobal*) malloc(sizeof(WasmGlobal));
    vector_push_back(module->globals, global);
    WasmExport* export1 = (WasmExport*) malloc(sizeof(WasmExport));
    export1->name = (char*) malloc(sizeof(char) * 6);
    strcpy(export1->name, "test1");
    export1->descType = Desc_Func;
    export1->descIdx = 0;
    vector_push_back(module->exports, export1);
    WasmExport* export2 = (WasmExport*) malloc(sizeof(WasmExport));
    export2->name = (char*) malloc(sizeof(char) * 6);
    strcpy(export2->name, "test2");
    export2->descType = Desc_Global;
    export2->descIdx = 0;
    vector_push_back(module->exports, export2);

    // Check
    ValueType* value = (ValueType*)malloc(sizeof(ValueType));
    *value = Value_i32;
    vector_push_back(type->results, value);
    EXPECT_EQ(validate_Module(module), -2);

    free_WasmModule(module);
}

SKYPAT_F(validate_Module, export_name_not_unique)
{
    // Prepare
    WasmModule* module = new_WasmModule(NULL);

    FuncType type = new_FuncType();
    vector_push_back(module->types, type);
    module->start = 0;
    WasmFunc* func = new_WasmFunc();
    func->type = 0;
    vector_push_back(module->funcs, func);
    WasmGlobal* global = (WasmGlobal*) malloc(sizeof(WasmGlobal));
    vector_push_back(module->globals, global);
    WasmExport* export1 = (WasmExport*) malloc(sizeof(WasmExport));
    export1->name = (char*) malloc(sizeof(char) * 6);
    strcpy(export1->name, "test1");
    export1->descType = Desc_Func;
    export1->descIdx = 0;
    vector_push_back(module->exports, export1);
    WasmExport* export2 = (WasmExport*) malloc(sizeof(WasmExport));
    export2->name = (char*) malloc(sizeof(char) * 6);
    strcpy(export2->name, "test1");
    export2->descType = Desc_Global;
    export2->descIdx = 0;
    vector_push_back(module->exports, export2);

    // Check
    EXPECT_EQ(validate_Module(module), -3);

    free_WasmModule(module);
}