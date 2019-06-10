#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <stdint.h>
#include <stdlib.h>
#include <dataTypes/Value.h>
#include <dataTypes/vector_p.h>
#include <dataTypes/FuncType.h>
#include <structures/WasmImport.h>
#include <Validates.h>
}
#undef _Bool

SKYPAT_F(Validate_Import, valid_func)
{
    // Prepare
    WasmImport* import = (WasmImport*)malloc(sizeof(WasmImport));
    WasmModule* module = new_WasmModule(NULL);
    FuncType funcType = new_FuncType();
    vector_push_back(module->types, funcType);
    import->descType = Desc_Func;
    import->desc.typeidx = 0;

    // Check
    EXPECT_EQ(validate_Import(import, module), 0);

    free(import);
    free_WasmModule(module);
}

SKYPAT_F(Validate_Import, func_type_not_defined)
{
    // Prepare
    WasmImport* import = (WasmImport*) malloc(sizeof(WasmImport));
    WasmModule* module = new_WasmModule(NULL);
    FuncType funcType = new_FuncType();
    vector_push_back(module->types, funcType);
    import->descType = Desc_Func;
    import->desc.typeidx = 1;

    // Check
    EXPECT_NE(validate_Import(import, module), 0);

    free(import);
    free_WasmModule(module);
}

SKYPAT_F(Validate_Import, limits_valid)
{
    // Prepare
    WasmImport* import = (WasmImport*) malloc(sizeof(WasmImport));
    WasmModule* module = new_WasmModule(NULL);
    import->descType = Desc_Table;
    import->desc.limits.min = 1;
    import->desc.limits.max = 3;

    // Check
    EXPECT_EQ(validate_Import(import, module), 0);
    import->descType = Desc_Mem;
    EXPECT_EQ(validate_Import(import, module), 0);

    free(import);
    free_WasmModule(module);
}

SKYPAT_F(Validate_Import, limits_max_smaller_than_min)
{
    // Prepare
    WasmImport* import = (WasmImport*) malloc(sizeof(WasmImport));
    WasmModule* module = new_WasmModule(NULL);
    import->descType = Desc_Table;
    import->desc.limits.min = 5;
    import->desc.limits.max = 3;

    // Check
    EXPECT_NE(validate_Import(import, module), 0);
    import->descType = Desc_Mem;
    EXPECT_NE(validate_Import(import, module), 0);

    free(import);
    free_WasmModule(module);
}

SKYPAT_F(Validate_Import, memory_limits_min_bigger_than_limit)
{
    // Prepare
    WasmImport* import = (WasmImport*) malloc(sizeof(WasmImport));
    WasmModule* module = new_WasmModule(NULL);
    import->descType = Desc_Mem;
    import->desc.limits.min = UINT16_MAX + 5;
    import->desc.limits.max = 0;

    // Check
    EXPECT_NE(validate_Import(import, module), 0);

    free(import);
    free_WasmModule(module);
}

SKYPAT_F(Validate_Import, memory_limits_max_bigger_than_limit)
{
    // Prepare
    WasmImport* import = (WasmImport*) malloc(sizeof(WasmImport));
    WasmModule* module = new_WasmModule(NULL);
    import->descType = Desc_Mem;
    import->desc.limits.min = 1;
    import->desc.limits.max = UINT16_MAX + 5;

    // Check
    EXPECT_NE(validate_Import(import, module), 0);

    free(import);
    free_WasmModule(module);
}
