#ifndef WASMVM_DECODER__DEF
#define WASMVM_DECODER__DEF

#include <Decoder.h>
#include <Stage.h>
#include <Loader.h>

struct Decoder_ {
    Stage       parent;         // inherit from Stage
    Loader     loader;
    Executor   executor;
};

#endif