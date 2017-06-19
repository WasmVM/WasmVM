#ifndef CONTROL_INST
#define CONTROL_INST

namespace Instruction{
  void ctrl_nop ();
  void ctrl_unreachable();
  void ctrl_block();
  void ctrl_loop();
  void ctrl_if();
  void ctrl_br();
  void ctrl_br_if();
  void ctrl_return();
  void ctrl_call();
  void ctrl_end();
}

#endif