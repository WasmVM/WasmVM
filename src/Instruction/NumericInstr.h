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

	void i32_eqz(Stack &coreStack);
	void i32_eq(Stack &coreStack);
	void i32_ne(Stack &coreStack);
	void i32_lt_s(Stack &coreStack);
	void i32_lt_u(Stack &coreStack);
	void i32_gt_s(Stack &coreStack);
	void i32_gt_u(Stack &coreStack);
	void i32_le_s(Stack &coreStack);
	void i32_le_u(Stack &coreStack);
	void i32_ge_s(Stack &coreStack);
	void i32_ge_u(Stack &coreStack);
	
	void i64_eqz(Stack &coreStack);
	void i64_eq(Stack &coreStack);
	void i64_ne(Stack &coreStack);
	void i64_lt_s(Stack &coreStack);
	void i64_lt_u(Stack &coreStack);
	void i64_gt_s(Stack &coreStack);
	void i64_gt_u(Stack &coreStack);
	void i64_le_s(Stack &coreStack);
	void i64_le_u(Stack &coreStack);
	void i64_ge_s(Stack &coreStack);
	void i64_ge_u(Stack &coreStack);

	void f32_eq(Stack &coreStack);
	void f32_ne(Stack &coreStack);
	void f32_lt(Stack &coreStack);
	void f32_gt(Stack &coreStack);
	void f32_le(Stack &coreStack);
	void f32_ge(Stack &coreStack);

	void f64_eq(Stack &coreStack);
	void f64_ne(Stack &coreStack);
	void f64_lt(Stack &coreStack);
	void f64_gt(Stack &coreStack);
	void f64_le(Stack &coreStack);
	void f64_ge(Stack &coreStack);
}

#endif