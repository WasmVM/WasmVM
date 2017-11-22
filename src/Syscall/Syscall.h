#ifndef SYSCALL_DEF
#define SYSCALL_DEF

extern "C"{
  #include <unistd.h>
  #include <errno.h>
}

#include <map>
#include <cstring>

#include <Loader.h>
#include <Instance.h>
#include <Exception.h>
#include <Store.h>
#include <Stack.h>
#include <Syscode.h>

class Syscall {
 public:
  static std::map<std::string, ModuleInst*>* moduleInsts;
  static void initSyscall(std::map<std::string, ModuleInst*>* moduleInsts);
  static void handle(Store& store, Stack& coreStack);
  static void sys_stdin(Store& store, Stack& coreStack);
  static void sys_stdout(Store& store, Stack& coreStack);
  static void sys_stderr(Store& store, Stack& coreStack);
};

#endif
