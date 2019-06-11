#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <string.h>
#include <Loader.h>
#include <Executor.h>
#include <core/Store.h>
#include <dataTypes/vector_p.h>
#include <instance/ModuleInst.h>
#include <LoaderRequest.h>
}
#undef _Bool

SKYPAT_F(LoaderRequest, new)
{
    // Prepare
    Loader loader = new_Loader();
    Executor executor = new_Executor();

    LoaderRequest request = new_LoaderRequest("TestModule", loader, executor);

    // Check
    EXPECT_FALSE(strcmp(request->moduleName, "TestModule"));
    EXPECT_EQ(queue_size(request->parent.stages), 3);

    free_LoaderRequest(request);
    free_Loader(loader);
    free_Executor(executor);
}
