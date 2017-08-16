(module 
	(func $addTwo1
		i32.const 63
		unreachable
		return
		i32.const 62
		unreachable
	)
	(func $start
		i32.const 61
		unreachable
		call $addTwo1
		i32.const 63
		unreachable
	)
	(start $start)
)
