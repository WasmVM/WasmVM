#include <skypat/skypat.h>

#define _Bool bool
extern "C" {
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <Loader.h>
#include <core/Store.h>
#include <dataTypes/vector.h>
#include <instance/ModuleInst.h>
#include <mock/MockLoaderRequest.h>
#include <mock/MockStage.h>
}
#undef _Bool

typedef struct {
    char input[10];
    Loader* loader;
} MockInput;

static int mock_activate_run(Stage* stage)
{
    MockInput* mockInput = (MockInput*)stage->input;
    sprintf((char*)stage->output, "Requests %d, Decoded: %d\n", mockInput->loader->requests->size, mockInput->loader->decodedStack->size);
    return 0;
}

static int mock_activate_halt(Stage* stage)
{
    MockInput* mockInput = (MockInput*)stage->input;
    sprintf((char*)stage->output, "Requests %d, Decoded: %d\n", mockInput->loader->requests->size, mockInput->loader->decodedStack->size);
    return 2;
}

static int mock_activate_terminate(Stage* stage)
{
    MockInput* mockInput = (MockInput*)stage->input;
    sprintf((char*)stage->output, "Requests %d, Decoded: %d\n", mockInput->loader->requests->size, mockInput->loader->decodedStack->size);
    mockInput->loader->parent.terminate((Component*)mockInput->loader);
    return 0;
}

SKYPAT_F(Loader, create_delete)
{
    // Prepare
    Loader* loader = new_Loader();

    // Check
    EXPECT_EQ(loader->loadedList->head, NULL);
    EXPECT_EQ(loader->loadedList->tail, NULL);
    EXPECT_EQ(loader->loadedList->size, 0);
    EXPECT_EQ(loader->decodedStack->head, NULL);
    EXPECT_EQ(loader->decodedStack->size, 0);
    EXPECT_EQ(loader->requests->head, NULL);
    EXPECT_EQ(loader->requests->tail, NULL);
    EXPECT_EQ(loader->requests->size, 0);

    free_Loader(loader);
}

SKYPAT_F(Loader, add_request)
{
    // Prepare
    Loader* loader = new_Loader();
    Store* store = new_Store();
    vector* moduleInsts = new_vector(sizeof(ModuleInst), (void(*)(void*))clean_ModuleInst);
    LoaderRequest* request = new_LoaderRequest("Test", (Component*)loader, store, moduleInsts);
    // Check
    loader->addRequest(loader, request);
    EXPECT_EQ(loader->requests->size, 1);
    EXPECT_EQ(loader->requests->head->data, request);

    // Clean
    free_Loader(loader);
    free_Store(store);
    free_vector(moduleInsts);
}

SKYPAT_F(Loader, activate)
{
    // Prepare
    Loader* loader = new_Loader();
    Store* store = new_Store();
    vector* moduleInsts = new_vector(sizeof(ModuleInst), (void(*)(void*))clean_ModuleInst);

    MockInput* mockInput1 = (MockInput*) malloc(sizeof(MockInput));
    strcpy(mockInput1->input, "Test1");
    mockInput1->loader = loader;
    LoaderRequest* request1 = new_MockLoaderRequest("Request1");
    char result1[100], result2[100];
    memset(result1, '\0', sizeof(char) * 100);
    memset(result2, '\0', sizeof(char) * 100);
    Stage* stage1 = new_MockStage((void*)mockInput1, (void*)result1, mock_activate_run);
    Stage* stage2 = new_MockStage((void*)mockInput1, (void*)result2, mock_activate_run);
    request1->parent.stages->push(request1->parent.stages, stage1);
    request1->parent.stages->push(request1->parent.stages, stage2);
    loader->addRequest(loader, request1);

    MockInput* mockInput2 = (MockInput*) malloc(sizeof(MockInput));
    strcpy(mockInput2->input, "Test2");
    mockInput2->loader = loader;
    LoaderRequest* request2 = new_MockLoaderRequest("Request2");
    char result3[100], result4[100];
    memset(result3, '\0', sizeof(char) * 100);
    memset(result4, '\0', sizeof(char) * 100);
    Stage* stage3 = new_MockStage((void*)mockInput2, (void*)result3, mock_activate_run);
    Stage* stage4 = new_MockStage((void*)mockInput2, (void*)result4, mock_activate_run);
    request2->parent.stages->push(request2->parent.stages, stage3);
    request2->parent.stages->push(request2->parent.stages, stage4);
    loader->addRequest(loader, request2);

    // Check
    loader->parent.activate((Component*)loader);
    int* resValue = NULL;
    pthread_join(loader->parent.thread, (void**)&resValue);
    EXPECT_EQ(strcmp(result1, "Requests 1, Decoded: 0\n"), 0);
    EXPECT_EQ(strcmp(result2, "Requests 0, Decoded: 0\n"), 0);
    EXPECT_EQ(strcmp(result3, "Requests 0, Decoded: 1\n"), 0);
    EXPECT_EQ(strcmp(result4, "Requests 0, Decoded: 1\n"), 0);
    EXPECT_EQ(loader->requests->size, 0);
    EXPECT_EQ(loader->decodedStack->size, 0);
    EXPECT_EQ(loader->loadedList->size, 2);
    EXPECT_FALSE(loader->parent.isTerminated);
    EXPECT_EQ(*resValue, 0);

    free_Loader(loader);
    free(resValue);
}

SKYPAT_F(Loader, activate_loaded)
{
    // Prepare
    Loader* loader = new_Loader();
    MockInput* mockInput1 = (MockInput*) malloc(sizeof(MockInput));
    strcpy(mockInput1->input, "Test1");
    mockInput1->loader = loader;
    LoaderRequest* request1 = new_MockLoaderRequest("Request1");
    char result1[100], result2[100];
    memset(result1, '\0', sizeof(char) * 100);
    memset(result2, '\0', sizeof(char) * 100);
    Stage* stage1 = new_MockStage((void*)mockInput1, (void*)result1, mock_activate_run);
    Stage* stage2 = new_MockStage((void*)mockInput1, (void*)result2, mock_activate_run);
    request1->parent.stages->push(request1->parent.stages, stage1);
    request1->parent.stages->push(request1->parent.stages, stage2);
    loader->addRequest(loader, request1);

    MockInput* mockInput2 = (MockInput*) malloc(sizeof(MockInput));
    strcpy(mockInput2->input, "Test1");
    mockInput2->loader = loader;
    LoaderRequest* request2 = new_MockLoaderRequest("Request1");
    char result3[100], result4[100];
    memset(result3, '\0', sizeof(char) * 100);
    memset(result4, '\0', sizeof(char) * 100);
    Stage* stage3 = new_MockStage((void*)mockInput2, (void*)result3, mock_activate_run);
    Stage* stage4 = new_MockStage((void*)mockInput2, (void*)result4, mock_activate_run);
    request2->parent.stages->push(request2->parent.stages, stage3);
    request2->parent.stages->push(request2->parent.stages, stage4);
    loader->addRequest(loader, request2);

    // Check
    loader->parent.activate((Component*)loader);
    int* resValue = NULL;
    pthread_join(loader->parent.thread, (void**)&resValue);
    EXPECT_EQ(strcmp(result1, "Requests 0, Decoded: 0\n"), 0);
    EXPECT_EQ(strcmp(result2, "Requests 0, Decoded: 0\n"), 0);
    EXPECT_EQ(strcmp(result3, ""), 0);
    EXPECT_EQ(strcmp(result4, ""), 0);
    EXPECT_EQ(loader->requests->size, 0);
    EXPECT_EQ(loader->decodedStack->size, 0);
    EXPECT_EQ(loader->loadedList->size, 1);
    EXPECT_FALSE(loader->parent.isTerminated);
    EXPECT_EQ(*resValue, 0);

    free_Loader(loader);
    free(resValue);
}

SKYPAT_F(Loader, activate_halt)
{
    // Prepare
    Loader* loader = new_Loader();
    MockInput* mockInput1 = (MockInput*) malloc(sizeof(MockInput));
    strcpy(mockInput1->input, "Test1");
    mockInput1->loader = loader;
    LoaderRequest* request1 = new_MockLoaderRequest("Request1");
    char result1[100], result2[100];
    memset(result1, '\0', sizeof(char) * 100);
    memset(result2, '\0', sizeof(char) * 100);
    Stage* stage1 = new_MockStage((void*)mockInput1, (void*)result1, mock_activate_run);
    Stage* stage2 = new_MockStage((void*)mockInput1, (void*)result2, mock_activate_run);
    request1->parent.stages->push(request1->parent.stages, stage1);
    request1->parent.stages->push(request1->parent.stages, stage2);
    loader->addRequest(loader, request1);

    MockInput* mockInput2 = (MockInput*) malloc(sizeof(MockInput));
    strcpy(mockInput2->input, "Test2");
    mockInput2->loader = loader;
    LoaderRequest* request2 = new_MockLoaderRequest("Request2");
    char result3[100], result4[100];
    memset(result3, '\0', sizeof(char) * 100);
    memset(result4, '\0', sizeof(char) * 100);
    Stage* stage3 = new_MockStage((void*)mockInput2, (void*)result3, mock_activate_run);
    Stage* stage4 = new_MockStage((void*)mockInput2, (void*)result4, mock_activate_halt);
    request2->parent.stages->push(request2->parent.stages, stage3);
    request2->parent.stages->push(request2->parent.stages, stage4);
    loader->addRequest(loader, request2);

    // Check
    loader->parent.activate((Component*)loader);
    int* resValue = NULL;
    pthread_join(loader->parent.thread, (void**)&resValue);
    EXPECT_EQ(strcmp(result1, "Requests 1, Decoded: 0\n"), 0);
    EXPECT_EQ(strcmp(result2, ""), 0);
    EXPECT_EQ(strcmp(result3, "Requests 0, Decoded: 1\n"), 0);
    EXPECT_EQ(strcmp(result4, "Requests 0, Decoded: 1\n"), 0);
    EXPECT_EQ(loader->requests->size, 0);
    EXPECT_EQ(loader->decodedStack->size, 1);
    EXPECT_EQ(loader->loadedList->size, 2);
    EXPECT_FALSE(loader->parent.isTerminated);
    EXPECT_EQ(*resValue, 2);

    free_Loader(loader);
    free(resValue);
}

SKYPAT_F(Loader, terminate)
{
    // Prepare
    Loader* loader = new_Loader();
    MockInput* mockInput1 = (MockInput*) malloc(sizeof(MockInput));
    strcpy(mockInput1->input, "Test1");
    mockInput1->loader = loader;
    LoaderRequest* request1 = new_MockLoaderRequest("Request1");
    char result1[100], result2[100];
    memset(result1, '\0', sizeof(char) * 100);
    memset(result2, '\0', sizeof(char) * 100);
    Stage* stage1 = new_MockStage((void*)mockInput1, (void*)result1, mock_activate_run);
    Stage* stage2 = new_MockStage((void*)mockInput1, (void*)result2, mock_activate_run);
    request1->parent.stages->push(request1->parent.stages, stage1);
    request1->parent.stages->push(request1->parent.stages, stage2);
    loader->addRequest(loader, request1);

    MockInput* mockInput2 = (MockInput*) malloc(sizeof(MockInput));
    strcpy(mockInput2->input, "Test2");
    mockInput2->loader = loader;
    LoaderRequest* request2 = new_MockLoaderRequest("Request2");
    char result3[100], result4[100];
    memset(result3, '\0', sizeof(char) * 100);
    memset(result4, '\0', sizeof(char) * 100);
    Stage* stage3 = new_MockStage((void*)mockInput2, (void*)result3, mock_activate_terminate);
    Stage* stage4 = new_MockStage((void*)mockInput2, (void*)result4, mock_activate_run);
    request2->parent.stages->push(request2->parent.stages, stage3);
    request2->parent.stages->push(request2->parent.stages, stage4);
    loader->addRequest(loader, request2);

    // Check
    loader->parent.activate((Component*)loader);
    int* resValue = NULL;
    pthread_join(loader->parent.thread, (void**)&resValue);
    EXPECT_EQ(strcmp(result1, "Requests 1, Decoded: 0\n"), 0);
    EXPECT_EQ(strcmp(result2, ""), 0);
    EXPECT_EQ(strcmp(result3, "Requests 0, Decoded: 1\n"), 0);
    EXPECT_EQ(strcmp(result4, ""), 0);
    EXPECT_EQ(loader->requests->size, 0);
    EXPECT_EQ(loader->decodedStack->size, 2);
    EXPECT_EQ(loader->loadedList->size, 2);
    EXPECT_TRUE(loader->parent.isTerminated);
    EXPECT_EQ(*resValue, 0);

    free_Loader(loader);
    free(resValue);
}
