(module
  (func $start
    loop (result i32)
    i32.const -10000
    drop
    i32.const 30
    br 1
    drop
    end
    i32.const 2
    i32.add
    drop
  )
  (start $start)
)
