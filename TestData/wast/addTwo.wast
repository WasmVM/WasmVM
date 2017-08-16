(module
  (func $addTwo (param i32 i32) (result i32)
    get_local 0
    get_local 1
    unreachable
    i32.const 40
  )
  (func $start
    i32.const 30
    i32.const 50
    call $addTwo
    unreachable
  )
  (export "addTwo" (func $addTwo))
  (start $start)
)
