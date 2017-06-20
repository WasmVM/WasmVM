#ifndef LOCAL_STACK
#define LOCAL_STACK

#include <stack>
#include <vector>

#include "commonTypes.h"

using namespace std;

class LocalIndex{
public:

  LocalIndex(uint16_t pc = 0, uint32_t index = 0):
    pc(pc),
    index(index){
  }
  uint64_t pc;
  uint32_t index;
};

class LocalStack{

public:
  LocalStack();
  Value get_local(uint32_t index);
  void set_local(uint32_t index, Value val);
  void push_local(Value val);
  Value tee_local(uint32_t index, Value val);

  void append_Values();
  void shrink_Values();
  void append_Indices();
  void shrink_Indices();
  void push_index(LocalIndex index);
  LocalIndex pop_index();
  uint64_t &get_PC();

#ifndef TEST
private:
#endif
  stack<vector<Value>> _localsValues;
  stack<stack<LocalIndex>> _localIndices;
};

#endif