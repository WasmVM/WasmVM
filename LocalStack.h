#ifndef LOCAL_STACK
#define LOCAL_STACK

#include <stack>
#include <vector>

#include "commonTypes.h"

using namespace std;

enum IndexType{i_function, i_if, i_block, i_else};

class LocalIndex{
public:
  LocalIndex(IndexType type, uint32_t index, uint64_t extra = 0):type(type), index(index), extra(extra){
  }
  IndexType type;
  uint32_t index;
  uint64_t extra;
};

class LocalIndices{
public:
  LocalIndices(uint64_t pc = 0):pc(pc){
    indices.push(LocalIndex(i_function, 0));
  }
  uint64_t pc;
  stack<LocalIndex> indices;
};

class LocalStack{

public:
  LocalStack();
  Value get_local(uint32_t index);
  void set_local(uint32_t index, Value val);
  void push_local(Value val);
  Value tee_local(uint32_t index, Value val);

  void append(uint64_t pc);
  bool shrink();
  void push_index(LocalIndex index);
  LocalIndex pop_index();
  uint64_t &get_PC();

#ifndef TEST
private:
#endif
  stack<vector<Value>> _localsValues;
  stack<LocalIndices> _localIndices;
};

#endif