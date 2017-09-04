#ifndef STORE_DEF
#define STORE_DEF

#include <Instance.h>
#include <vector>

class Store {
 public:
  ~Store();
  std::vector<FuncInst*> funcs;
  std::vector<TableInst*> tables;
  std::vector<MemInst*> mems;
  std::vector<GlobalInst*> globals;
};

#endif