#ifndef WASMVM_TEST_UNITTESTS_MOCK_MOCKLOADERREQUEST_DEF
#define WASMVM_TEST_UNITTESTS_MOCK_MOCKLOADERREQUEST_DEF

#include <stdlib.h>
#include <string.h>
#include <dataTypes/queue.h>
#include <LoaderRequest.h>
#include <Stage.h>

void mock_request_free(Request* request)
{
    free_queue(request->stages);
    free(request);
}

LoaderRequest* new_MockLoaderRequest(const char* name)
{
    LoaderRequest* newRequest = (LoaderRequest*) malloc(sizeof(LoaderRequest));
    newRequest->parent.stages = new_queue(free);
    newRequest->parent.free = mock_request_free;
    newRequest->moduleName = (char*) malloc(sizeof(char) * strlen(name));
    strcpy(newRequest->moduleName, name);
    return newRequest;
}

#endif
