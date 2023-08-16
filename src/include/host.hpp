#ifndef WASMVM_PP_HOST_DEF
#define WASMVM_PP_HOST_DEF

#include <instances/ModuleInst.hpp>
#include <instances/Store.hpp>
#include <map>
#include <filesystem>

using namespace WasmVM;

void host_modules_instanciate(std::map<std::filesystem::path, ModuleInst>& moduleinsts, Store& store);

#endif