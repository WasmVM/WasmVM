(module
	(func $add
		i32.const 63
		drop
  )
  (func $start
		call $add
		i32.const 62
		drop
  )
  (start $start)
)
