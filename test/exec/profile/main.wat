(module
  (import "lib" "g" (func $g (result i32)))
  (import "sys_proc" "argc" (func $argc (result i32)))
  (func $fib (param i32) (result i32)
    local.get 0 i32.const 2 i32.lt_s
    if (result i32)
      local.get 0
    else
      local.get 0 i32.const 1 i32.sub call $fib
      local.get 0 i32.const 2 i32.sub call $fib
      i32.add
    end)
  (func $main
    i32.const 5 call $fib drop
    call $g drop
    call $argc drop)
  (start $main))
