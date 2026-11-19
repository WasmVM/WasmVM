(module
  (func $a i32.const 1 drop unreachable)
  (func $main call $a)
  (start $main))
