#ifndef WASMVM_PP_DEF
#define WASMVM_PP_DEF

#include <string>
#include <vector>
#include <cstddef>
#include <iostream>
#include <optional>
#include <map>
#include <structures/WasmModule.hpp>
#include <exception.hpp>
#include <instances/Store.hpp>
#include <instances/ModuleInst.hpp>
#include <instances/Stack.hpp>

#define WASMVM_VERSION "v1.7"

namespace WasmVM {

WasmModule module_parse(std::istream& stream);
std::ostream& module_dump(const WasmModule& module, std::ostream& stream);
std::ostream& module_encode(const WasmModule& module, std::ostream& stream);
WasmModule module_decode(std::istream& stream);
std::optional<Exception::Exception> module_validate(const WasmModule& module);
/// \param observers Optional; the start function runs under them (see Observers in <instances/Stack.hpp>).
ModuleInst& module_instanciate(ModuleInst& moduleInst, Store& store, const WasmModule& module, std::vector<ExternVal> externval, Observers observers = {});

}

#endif