#ifndef WASMVM_TEST_UNITTESTS_MOCK_MOCKREQUEST_DEF
#define WASMVM_TEST_UNITTESTS_MOCK_MOCKREQUEST_DEF

#include <stdlib.h>
#include <dataTypes/queue.h>
#include <Request.h>
#include <Stage.h>

void mock_request_free(Request* request)
{
    free_queue(request->stages);
    free(request);
}

Request* new_MockRequest()
{
    Request* newRequest = (Request*) malloc(sizeof(Request));
    newRequest->stages = new_queue(free);
    newRequest->free = mock_request_free;
    return newRequest;
}

#endif
