#ifndef TABLEINST_DEF
#define TABLEINST_DEF

#include <cstdint>
#include <Value.h>

class TableInst {
 public:
  std::vector<uint32_t> elem;
  uint32_t max;
};

#endif