(module
  (import "sys_proc" "exit" (func $exit (param i32)))
  (func $a i32.const 3 call $exit)
  (func $main call $a)
  (start $main))
