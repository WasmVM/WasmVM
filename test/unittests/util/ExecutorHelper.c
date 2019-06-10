#include "ExecutorHelper.h"

#include <stdatomic.h>
#include <Executor_.h>

int executor_get_runningCores(Executor executor)
{
    return atomic_load(&(executor->runningCores));
}
vector_p executor_get_cores(Executor executor)
{
    return executor->cores;
}