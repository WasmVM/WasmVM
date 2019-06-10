#ifndef WASMVM_TEST_UNITTESTS_UTIL_EXECUTORHELPER_DEF
#define WASMVM_TEST_UNITTESTS_UTIL_EXECUTORHELPER_DEF

#include <Executor.h>
#include <dataTypes/vector_p.h>

int executor_get_runningCores(Executor executor);
vector_p executor_get_cores(Executor executor);

#endif