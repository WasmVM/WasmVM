#include <ControlInstr.h>

void Instruction::invoke(std::uint32_t funcAddr, Store &store, Stack &coreStack){
	if(funcAddr > store.funcs.size()){
		throw ControlException("invoke","Function address not exist.", coreStack);
	}
	FuncInst *funcInst = store.funcs.at(funcAddr);
	if(coreStack.valueCount() < funcInst->type.paramTypes.size()){
		throw ControlException("invoke","No enough values for the function parameters", coreStack);
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
	coreStack.refreshLabel();
}