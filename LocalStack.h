#ifndef LOCAL_STACK
#define LOCAL_STACK

#include <stack>
#include <vector>

#include "commonTypes.h"

using namespace std;

class LocalStack{

public:
  LocalStack();
  Value get_local(uint32_t index);
  void set_local(uint32_t index, Value val);
  Value tee_local(uint32_t index, Value val);

  void append_Values();
  void pop_Values();
  void append_Indices();
  void pop_Indices();

#ifndef TEST
private:
#endif
  stack<vector<Value>> _localsValues;
  stack<uint32_t> _localIndices;
};

#endif