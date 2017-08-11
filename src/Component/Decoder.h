#ifndef DECODER_DEF
#define DECODER_DEF

#include <Instance.h>
#include <Store.h>
#include <Stack.h>
#include <Instruction.h>
#include <Exceptions.h>

namespace Decoder {
    void decode(Store &store, Stack &coreStack);
}

#endif