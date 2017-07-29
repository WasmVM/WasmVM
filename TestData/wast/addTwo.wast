(module
  (func $addTwo (param i32 i32) (result i32)
    get_local 0
    i32.const 7
    tee_local 1
    i32.add)
  (func $start
    i32.const 3
    i32.const 5
    call $addTwo
    drop
  )
  (export "addTwo" (func $addTwo))
  (start $start)
)
