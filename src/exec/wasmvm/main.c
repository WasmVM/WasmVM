#include <stdio.h>

#include <Executor.h>
#include <Loader.h>
#include <LoaderRequest.h>

void print_usage()
{
    fprintf(stderr, "Usage: wasmvm <main_module.wasm>\n");
}

int main(int argc, char const *argv[])
{
    // Options
    // TODO: More options
    if(argc != 2) {
        print_usage();
        return -1;
    }
    // Components
    Loader loader = new_Loader();
    Executor executor = new_Executor();
    // Request
    LoaderRequest request = new_LoaderRequest(argv[1], loader, executor);
    loader_addRequest(loader, request);
    // Run Loader
    loader_activate(loader);
    // Join Loader
    int result = 0;
    int* resultPtr = &result;
    if(loader_join(loader, &resultPtr) || result) {
        free_Loader(loader);
        free_Executor(executor);
        return -1;
    }
    // Run Executor
    if((result = executor_run(executor)) == 0) {
        result = executor_join(executor);
    }
    // Clean
    free_Loader(loader);
    free_Executor(executor);
    return result;
}
