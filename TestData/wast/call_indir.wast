(module
  (table 0 1 anyfunc)
  (elem 0 (offset (i32.const 0)) 0)
  (func
		i32.const 30
		unreachable
  )
  (func $start
    i32.const 0
    call_indirect 0
  )
  (start $start)
)
