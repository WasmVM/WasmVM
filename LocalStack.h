#ifndef LOCAL_STACK
#define LOCAL_STACK

#include <stack>
#include <vector>

#include "commonTypes.h"

class LocalStack
{
public:
  LocalStack();
  Value get_local(uint32_t index);
  void set_local(uint32_t index, Value val);
  Value tee_local(uint32_t index, Value val);

  void appendValues();
  void popValues();
  void appendIndices();
  void popIndices();

private:
  stack<vector<Value>> _localsValues;
  stack<uint32_t> _localIndices;
};


#endif