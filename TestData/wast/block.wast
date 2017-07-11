(module
  (func $start
    block
    i32.const 20
    drop
    i32.const 1
    br_if 1
    i32.const 30
    drop
    end
  )
  (start $start)
)
