(module
  (func $addTwo
    i32.const 40
  	i32.const 60
  	i32.add
    drop
  )
  (func $start
  	i32.const -10000
  	i32.const -20000
  	i32.add
  	drop
  	i32.const 0
  	unreachable
  	drop
  	call $addTwo
  )
  (start $start)
  (export "addTwo" (func $addTwo)))
