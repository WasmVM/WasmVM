(module
  (func $start
    i32.const 0
    if
    i32.const 5
    i32.const 30
    i32.add
    drop
    else
    i32.const 70
    i32.const 50
    i32.sub
    drop
    end
  )
  (start $start)
)
