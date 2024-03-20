#ifndef WASMVM_PP_HOST_DEF
#define WASMVM_PP_HOST_DEF

#include <instances/ModuleInst.hpp>
#include <instances/Store.hpp>
#include <map>
#include <filesystem>

#if defined(WIN32)
  #if defined(HOST_API_DEF)
    #define LIBRARY_API __declspec(dllexport)
  #else
    #define LIBRARY_API __declspec(dllimport)
  #endif
#else
  #define LIBRARY_API
#endif

using namespace WasmVM;

LIBRARY_API void host_modules_instanciate(std::map<std::filesystem::path, ModuleInst>& moduleinsts, Store& store);

#endif