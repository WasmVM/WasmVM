#ifndef MODULEINST_DEF
#define MODULEINST_DEF

#include <cstdint>
#include <vector>
#include <Value.h>
#include <ExportInst.h>

class ModuleInst {
 public:
  std::vector<FuncType> types;
  std::vector<uint32_t> funcaddrs;
  std::vector<uint32_t> tableaddrs;
  std::vector<uint32_t> memaddrs;
  std::vector<uint32_t> globaladdrs;
  std::vector<ExportInst> exports;
  std::uint32_t* start;
};

#endif