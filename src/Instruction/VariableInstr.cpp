#include <VariableInstr.h>

void Instruction::get_local(std::uint32_t index, Stack &coreStack){
	// Check local
	if(index >= coreStack.curFrame->locals.size()){
		throw Exception("[get_local] No such local variable in this frame.", coreStack);
	}
	// Push value
	coreStack.push(coreStack.curFrame->locals.at(index));
}
void Instruction::set_local(std::uint32_t index, Stack &coreStack){
	// Check local
	if(index >= coreStack.curFrame->locals.size()){
		throw Exception("[set_local] No such local variable in this frame.", coreStack);
	}
	// Check value
	if(coreStack.valueCount() < 1){
		throw Exception("[set_local] No value in the stack.", coreStack);
	}
	// Pop value
	Value *val = (Value *)coreStack.pop().data;
	// Replace
	coreStack.curFrame->locals.at(index) = *val;
	// Clean
	delete val;
}
void Instruction::tee_local(std::uint32_t index, Stack &coreStack){
	// Check local
	if(index >= coreStack.curFrame->locals.size()){
		throw Exception("[tee_local] No such local variable in this frame.", coreStack);
	}
	// Check value
	if(coreStack.valueCount() < 1){
		throw Exception("[tee_local] No value in the stack.", coreStack);
	}
	// Get value
	Value *val = (Value *)coreStack.top().data;
	// Replace
	coreStack.curFrame->locals.at(index) = *val;
}
void Instruction::get_global(std::uint32_t index, Store &store, Stack &coreStack){
	// Check global index
	if(index >= coreStack.curFrame->moduleInst->globaladdrs.size()){
		throw Exception("[get_global] No such global address in this frame.", coreStack);
	}
	std::uint32_t globalAddr = coreStack.curFrame->moduleInst->globaladdrs.at(index);
	// Check global
	if(globalAddr >= store.globals.size()){
		throw Exception("[get_global] No such global variable in the store.", coreStack);
	}
	// Push value
	coreStack.push(store.globals.at(globalAddr)->val);
}
void Instruction::set_global(std::uint32_t index, Store &store, Stack &coreStack){
	// Check global index
	if(index >= coreStack.curFrame->moduleInst->globaladdrs.size()){
		throw Exception("[set_global] No such global address in this frame.", coreStack);
	}
	std::uint32_t globalAddr = coreStack.curFrame->moduleInst->globaladdrs.at(index);
	// Check global
	if(globalAddr >= store.globals.size()){
		throw Exception("[set_global] No such global variable in the store.", coreStack);
	}
	// Pop value
	Value *val = (Value *)coreStack.pop().data;
	// Replace
	store.globals.at(globalAddr)->val = *val;
	// Clean
	delete val;
}