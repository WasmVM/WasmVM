#ifndef NUMERICINSTR_DEF
#define NUMERICINSTR_DEF

#include <cstdint>
#include <Stack.h>
#include <Store.h>
#include <Instance.h>
#include <Exception.h>

namespace Instruction{
	void i32_const(Stack &coreStack, std::int32_t);
	void i64_const(Stack &coreStack, std::int64_t);
	void f32_const(Stack &coreStack, float);
	void f64_const(Stack &coreStack, double);
}

#endif