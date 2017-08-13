(module 
	(func $addTwo1 (local i32)
		i32.const 63
		drop
		return
		i32.const 62
		drop
	)
	(func $start
		i32.const 61
		drop
		call $addTwo1
		i32.const 63
		drop
	)
	(start $start)
)
