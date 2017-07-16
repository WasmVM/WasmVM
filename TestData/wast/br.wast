(module
  (func $start
    loop
    i32.const 20
    drop
    i32.const 0
    br_if 1
    end
  )
  (start $start)
)
