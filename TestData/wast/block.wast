(module
  (func $start
    block (result i32)
    i32.const 2
    i32.const 3
    i32.add
    end
    drop
  )
  (export "start" (func $start))
  (start $start)
)
