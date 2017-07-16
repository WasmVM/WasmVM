(module
  (func $start
    i32.const 0
    if (result i32)
    i32.const 5
    i32.const 30
    i32.add
    else
    i32.const 70
    i32.const 50
    i32.sub
    end
    drop
  )
  (start $start)
)
