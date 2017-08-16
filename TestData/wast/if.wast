(module
  (func $start
    i32.const 1
    if (result i32)
    i32.const 5
    else
    i32.const 70
    end
    unreachable
  )
  (start $start)
)
