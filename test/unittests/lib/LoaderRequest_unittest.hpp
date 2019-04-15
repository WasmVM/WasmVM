#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <string.h>
#include <Loader.h>
#include <core/Store.h>
#include <dataTypes/vector.h>
#include <instance/ModuleInst.h>
#include <LoaderRequest.h>
}
#undef _Bool

SKYPAT_F(LoaderRequest, new)
{
    // Prepare
    Loader* loader = new_Loader();
    Store* store = new_Store();
    vector* moduleInsts = new_vector(sizeof(ModuleInst), (void(*)(void*))clean_ModuleInst);

    LoaderRequest* request = new_LoaderRequest("TestModule", (Component*)loader, store, moduleInsts);

    // Check
    EXPECT_FALSE(strcmp(request->moduleName, "TestModule"));
    EXPECT_EQ(request->parent.stages->size, 3);

    free_LoaderRequest(request);
    free_Loader(loader);
    free_Store(store);
    free_vector(moduleInsts);
}
