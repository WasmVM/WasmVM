#ifndef WASMVM_INSTANCIATOR__DEF
#define WASMVM_INSTANCIATOR__DEF

#include <Stage.h>
#include <Executor.h>
#include <Instanciator.h>

struct Instanciator_ {
    Stage parent;
    Executor executor;
};

#endif