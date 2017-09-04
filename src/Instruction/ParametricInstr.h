#ifndef PARAMETRICINSTR_DEF
#define PARAMETRICINSTR_DEF

#include <cstdint>
#include <Stack.h>
#include <Exception.h>

namespace Instruction {
void drop(Stack& coreStack);
void select(Stack& coreStack);
}

#endif