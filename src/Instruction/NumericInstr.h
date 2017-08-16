#ifndef NUMERICINSTR_DEF
#define NUMERICINSTR_DEF

#include <cstdint>
#include <Stack.h>
#include <Instance.h>
#include <Util.h>

namespace Instruction{
	void i32_const(Stack &coreStack, std::int32_t val);
	void i64_const(Stack &coreStack, std::int64_t val);
	void f32_const(Stack &coreStack, float val);
	void f64_const(Stack &coreStack, double val);
}

#endif