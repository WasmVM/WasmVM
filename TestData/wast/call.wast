(module
	(func $add
		i32.const 30
		drop
  )
  (func $start
		call $add
  )
  (start $start)
)
