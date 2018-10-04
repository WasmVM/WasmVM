#include <MemoryInstr.h>

void Instruction::i32_load(std::uint32_t align, std::uint32_t offset, Store &store, Stack &coreStack){
	// Get data pointer
	char *ptr = Util::getMemoryPtr(store, coreStack, align, offset, 4, "[i32.load] ");
	// Parse & push value
	std::int32_t value = Util::toLittle32(*(std::int32_t *)ptr);
	coreStack.push(Value(value));
}
void Instruction::i64_load(std::uint32_t align, std::uint32_t offset, Store &store, Stack &coreStack){
	// Get data pointer
	char *ptr = Util::getMemoryPtr(store, coreStack, align, offset, 8, "[i64.load] ");
	// Parse & push value
	std::int64_t value = Util::toLittle64(*(std::int64_t *)ptr);
	coreStack.push(Value(value));
}
void Instruction::f32_load(std::uint32_t align, std::uint32_t offset, Store &store, Stack &coreStack){
	// Get data pointer
	char *ptr = Util::getMemoryPtr(store, coreStack, align, offset, 4, "[f32.load] ");
	// Parse & push value
	std::uint32_t val32 = Util::toLittle32(*(std::int32_t *)ptr);
	float value = *(float *)&val32;
	coreStack.push(Value(value));
}
void Instruction::f64_load(std::uint32_t align, std::uint32_t offset, Store &store, Stack &coreStack){
	// Get data pointer
	char *ptr = Util::getMemoryPtr(store, coreStack, align, offset, 8, "[f64.load] ");
	// Parse & push value
	std::uint64_t val64 = Util::toLittle64(*(std::int64_t *)ptr);
	double value = *(double *)&val64;
	coreStack.push(Value(value));
}
void Instruction::i32_load8_s(std::uint32_t align, std::uint32_t offset, Store &store, Stack &coreStack){
	// Get data pointer
	char *ptr = Util::getMemoryPtr(store, coreStack, align, offset, 1, "[i32.load8_s] ");
	// Parse & push value
	coreStack.push(Value((std::int32_t)*ptr));
}
void Instruction::i32_load8_u(std::uint32_t align, std::uint32_t offset, Store &store, Stack &coreStack){
	// Get data pointer
	char *ptr = Util::getMemoryPtr(store, coreStack, align, offset, 1, "[i32.load8_u] ");
	// Parse & push value
	std::int32_t val32 = (std::int32_t)*ptr & 0xFF;
	coreStack.push(Value(val32));
}
void Instruction::i32_load16_s(std::uint32_t align, std::uint32_t offset, Store &store, Stack &coreStack){
	// Get data pointer
	char *ptr = Util::getMemoryPtr(store, coreStack, align, offset, 2, "[i32.load16_s] ");
	// Parse & push value
	std::int32_t val32 = Util::toLittle16(*(std::int16_t *)ptr);
	if(val32 & 0x8000){
		val32 |= 0xFFFF0000;
	}
	coreStack.push(Value(val32));
}
void Instruction::i32_load16_u(std::uint32_t align, std::uint32_t offset, Store &store, Stack &coreStack){
	// Get data pointer
	char *ptr = Util::getMemoryPtr(store, coreStack, align, offset, 2, "[i32.load16_u] ");
	// Parse & push value
	std::int32_t val32 = Util::toLittle16(*(std::int16_t *)ptr);
	coreStack.push(Value(val32));
}
void Instruction::i64_load8_s(std::uint32_t align, std::uint32_t offset, Store &store, Stack &coreStack){
	// Get data pointer
	char *ptr = Util::getMemoryPtr(store, coreStack, align, offset, 1, "[i64.load8_s] ");
	// Parse & push value
	coreStack.push(Value((std::int64_t)*ptr));
}
void Instruction::i64_load8_u(std::uint32_t align, std::uint32_t offset, Store &store, Stack &coreStack){
	// Get data pointer
	char *ptr = Util::getMemoryPtr(store, coreStack, align, offset, 1, "[i64.load8_u] ");
	// Parse & push value
	std::int64_t val64 = (std::int64_t)*ptr & 0xFF;
	coreStack.push(Value(val64));
}
void Instruction::i64_load16_s(std::uint32_t align, std::uint32_t offset, Store &store, Stack &coreStack){
	// Get data pointer
	char *ptr = Util::getMemoryPtr(store, coreStack, align, offset, 2, "[i64.load16_s] ");
	// Parse & push value
	std::int64_t val64 = Util::toLittle16(*(std::int16_t *)ptr);
	if(val64 & 0x8000){
		val64 |= 0xFFFFFFFFFFFF0000;
	}
	coreStack.push(Value(val64));
}
void Instruction::i64_load16_u(std::uint32_t align, std::uint32_t offset, Store &store, Stack &coreStack){
	// Get data pointer
	char *ptr = Util::getMemoryPtr(store, coreStack, align, offset, 2, "[i64.load16_u] ");
	// Parse & push value
	std::int64_t val64 = Util::toLittle16(*(std::int16_t *)ptr);
	coreStack.push(Value(val64));
}
void Instruction::i64_load32_s(std::uint32_t align, std::uint32_t offset, Store &store, Stack &coreStack){
	// Get data pointer
	char *ptr = Util::getMemoryPtr(store, coreStack, align, offset, 4, "[i64.load32_s] ");
	// Parse & push value
	std::int64_t val64 = Util::toLittle32(*(std::int32_t *)ptr);
	if(val64 & 0x80000000){
		val64 |= 0xFFFFFFFF00000000;
	}
	coreStack.push(Value(val64));
}
void Instruction::i64_load32_u(std::uint32_t align, std::uint32_t offset, Store &store, Stack &coreStack){
	// Get data pointer
	char *ptr = Util::getMemoryPtr(store, coreStack, align, offset, 4, "[i64.load32_u] ");
	// Parse & push value
	std::int64_t val64 = Util::toLittle32(*(std::int32_t *)ptr);
	coreStack.push(Value(val64));
}
void Instruction::i32_store(std::uint32_t align, std::uint32_t offset, Store &store, Stack &coreStack){
	// Check operand
	if(coreStack.valueNum < 1){
		throw Exception("[i32.store] No value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != i32){
		throw Exception("[i32.store] Value type is not i32.", coreStack);
	}
	// Get data pointer
	char *ptr = Util::getMemoryPtr(store, coreStack, align, offset, 4, "[i32.store] ");
	// Store value
	*((std::int32_t *)ptr) = operand->data.i32;
	// Clean
	delete operand;
}
void Instruction::i64_store(std::uint32_t align, std::uint32_t offset, Store &store, Stack &coreStack){
	// Check operand
	if(coreStack.valueNum < 1){
		throw Exception("[i64.store] No value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != i64){
		throw Exception("[i64.store] Value type is not i64.", coreStack);
	}
	// Get data pointer
	char *ptr = Util::getMemoryPtr(store, coreStack, align, offset, 8, "[i64.store] ");
	// Store value
	*((std::int64_t *)ptr) = operand->data.i64;
	// Clean
	delete operand;
}
void Instruction::f32_store(std::uint32_t align, std::uint32_t offset, Store &store, Stack &coreStack){
	// Check operand
	if(coreStack.valueNum < 1){
		throw Exception("[f32.store] No value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != f32){
		throw Exception("[f32.store] Operand type is not f32.", coreStack);
	}
	// Get data pointer
	char *ptr = Util::getMemoryPtr(store, coreStack, align, offset, 4, "[f32.store] ");
	// Store value
	*((float *)ptr) = operand->data.f32;
	// Clean
	delete operand;
}
void Instruction::f64_store(std::uint32_t align, std::uint32_t offset, Store &store, Stack &coreStack){
	// Check operand
	if(coreStack.valueNum < 1){
		throw Exception("[f64.store] No value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != f64){
		throw Exception("[f64.store] Operand type is not f64.", coreStack);
	}
	// Get data pointer
	char *ptr = Util::getMemoryPtr(store, coreStack, align, offset, 8, "[f64.store] ");
	// Store value
	*((double *)ptr) = operand->data.f64;
	// Clean
	delete operand;
}
void Instruction::i32_store8(std::uint32_t align, std::uint32_t offset, Store &store, Stack &coreStack){
	// Check operand
	if(coreStack.valueNum < 1){
		throw Exception("[i32.store8] No value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != i32){
		throw Exception("[i32.store8] Value type is not i32.", coreStack);
	}
	// Get data pointer
	char *ptr = Util::getMemoryPtr(store, coreStack, align, offset, 1, "[i32.store8] ");
	// Store value
	*((std::int8_t *)ptr) = operand->data.i32;
	// Clean
	delete operand;
}
void Instruction::i32_store16(std::uint32_t align, std::uint32_t offset, Store &store, Stack &coreStack){
	// Check operand
	if(coreStack.valueNum < 1){
		throw Exception("[i32.store16] No value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != i32){
		throw Exception("[i32.store16] Value type is not i32.", coreStack);
	}
	// Get data pointer
	char *ptr = Util::getMemoryPtr(store, coreStack, align, offset, 2, "[i32.store16] ");
	// Store value
	*((std::int16_t *)ptr) = operand->data.i32;
	// Clean
	delete operand;
}
void Instruction::i64_store8(std::uint32_t align, std::uint32_t offset, Store &store, Stack &coreStack){
	// Check operand
	if(coreStack.valueNum < 1){
		throw Exception("[i64.store8] No value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != i64){
		throw Exception("[i64.store8] Value type is not i64.", coreStack);
	}
	// Get data pointer
	char *ptr = Util::getMemoryPtr(store, coreStack, align, offset, 1, "[i64.store8] ");
	// Store value
	*((std::int8_t *)ptr) = operand->data.i64;
	// Clean
	delete operand;
}
void Instruction::i64_store16(std::uint32_t align, std::uint32_t offset, Store &store, Stack &coreStack){
	// Check operand
	if(coreStack.valueNum < 1){
		throw Exception("[i64.store16] No value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != i64){
		throw Exception("[i64.store16] Value type is not i64.", coreStack);
	}
	// Get data pointer
	char *ptr = Util::getMemoryPtr(store, coreStack, align, offset, 2, "[i64.store16] ");
	// Store value
	*((std::int16_t *)ptr) = operand->data.i64;
	// Clean
	delete operand;
}
void Instruction::i64_store32(std::uint32_t align, std::uint32_t offset, Store &store, Stack &coreStack){
	// Check operand
	if(coreStack.valueNum < 1){
		throw Exception("[i64.store32] No value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != i64){
		throw Exception("[i64.store32] Value type is not i64.", coreStack);
	}
	// Get data pointer
	char *ptr = Util::getMemoryPtr(store, coreStack, align, offset, 4, "[i64.store32] ");
	// Store value
	*((std::int32_t *)ptr) = operand->data.i64;
	// Clean
	delete operand;
}
void Instruction::current_memory(Store &store, Stack &coreStack){
	// Bypass reserved
	coreStack.curLabel->instrOffset += 1;
	// Check memory address
	if(coreStack.curFrame->moduleInst->memaddrs.size() < 1){
		throw Exception("[current_memory] No memory exists in this module.", coreStack);
	}
	// Check memory
	std::uint32_t memAddr = coreStack.curFrame->moduleInst->memaddrs.at(0);
	if(memAddr >= store.mems.size()){
		throw Exception("[current_memory] Memory not exists in the store.", coreStack);
	}
	MemInst *memory = store.mems.at(memAddr);
	// Push memory size
	coreStack.push(Value((std::int32_t)(memory->data.size() / 65536)));
}
void Instruction::grow_memory(Store &store, Stack &coreStack){
	// Bypass reserved
	coreStack.curLabel->instrOffset += 1;
	// Check memory address
	if(coreStack.curFrame->moduleInst->memaddrs.size() < 1){
		throw Exception("[grow_memory] No memory exists in this module.", coreStack);
	}
	// Check memory
	std::uint32_t memAddr = coreStack.curFrame->moduleInst->memaddrs.at(0);
	if(memAddr >= store.mems.size()){
		throw Exception("[grow_memory] Memory not exists in the store.", coreStack);
	}
	MemInst *memory = store.mems.at(memAddr);
	// Get memory size
	std::int32_t memSize = memory->data.size() / 65536;
	// Check operand
	if(coreStack.valueNum < 1){
		throw Exception("[grow_memory] No value in the stack.", coreStack);
	}
	// Pop operand
	Value *operand = (Value *)coreStack.pop().data;
	if(operand->type != i32){
		throw Exception("[grow_memory] Operand type is not i32.", coreStack);
	}
	// Grow
	std::int32_t n = memSize + operand->data.i32;
	if(memory->max && n > (std::int32_t)memory->max){
		coreStack.push(Value((std::int32_t)-1));
	}else{
		try{
			if(n > 0){
				memory->data.resize(n * 65536);
			}else{
				memory->data.resize(0);
			}
			coreStack.push(Value(memSize));
		}catch(std::bad_alloc){
			coreStack.push(Value((std::int32_t)-1));
		}
	}
	// Clean
	delete operand;
}
