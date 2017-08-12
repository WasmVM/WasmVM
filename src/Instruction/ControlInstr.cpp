#include <ControlInstr.h>

void Instruction::invoke(std::uint32_t funcAddr, Store &store, Stack &coreStack, ModuleInst *moduleInst){
	if(funcAddr > store.funcs.size()){
		throw Exception("[invoke] Function address not exist.", coreStack, moduleInst);
	}
	FuncInst *funcInst = store.funcs.at(funcAddr);
	if(coreStack.valueCount() < funcInst->type.paramTypes.size()){
		throw Exception("[invoke] No enough values for function parameters", coreStack, moduleInst);
	}
	Frame newFrame;
	newFrame.moduleInst = funcInst->module;
	newFrame.locals.resize(funcInst->type.paramTypes.size() + funcInst->code.localTypes.size());
	// Set params
	for(size_t i = 0; i < funcInst->type.paramTypes.size(); ++i){
		Value *val = (Value *)coreStack.pop().data;
		newFrame.locals.at(i) = *val;
		delete val;
	}
	// Set locals
	for(size_t i = 0, j = funcInst->type.paramTypes.size(); i < funcInst->code.localTypes.size(); ++i, ++j){
		newFrame.locals.at(j).type = funcInst->code.localTypes.at(i);
		newFrame.locals.at(j).data.i64 = 0;
	}
	// Push frame to stack
	coreStack.push(newFrame);
	// Set label
	Label newLabel;
	newLabel.funcIdx = funcAddr;
	newLabel.resultTypes = funcInst->type.resultTypes;
	// Push label to stack
	coreStack.push(newLabel);
}

void Instruction::ctrl_end(Stack &coreStack){
	// Check result count
	size_t resCount = coreStack.curLabel->resultTypes.size();
	if(coreStack.valueCount() != resCount){
		if(coreStack.valueCount() > resCount){
			throw Exception("[end] Too many values left in the stack while block ending.", coreStack);
		}else{
			throw Exception("[end] Too few values left in the stack for the block result.", coreStack);
		}
	}
	// Pop values
	std::vector<Value> resultValues;
	for(size_t i = 0; i < resCount; ++i){
		resultValues.push_back(*((Value *)coreStack.pop().data));
	}
	// Pop label
	if(coreStack.pop().type != StackElemType::label){
		throw Exception("[end] There must be a label on the top of stack after popping values.", coreStack);
	}
	// Return from function
	if(coreStack.top().type == StackElemType::frame){
		coreStack.pop();
	}
	// Push values
	for(size_t i = 0; i < resCount; ++i){
		coreStack.push(resultValues.back());
		resultValues.pop_back();
	}
}