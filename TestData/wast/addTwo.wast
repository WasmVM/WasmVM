(module
  (func $addTwo (param i32 i32) (result i32)
    get_local 1
    i32.const 62
    tee_local 1
    i32.add)
  (func $start
    i32.const 62
    i32.const 63
    call $addTwo
    drop
  )
  (export "addTwo" (func $addTwo))
  (start $start)
)
