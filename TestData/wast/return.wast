(module 
	(func $addTwo1 (result i32)
		i32.const 3
		return
		i32.const 5
		i32.add
	)
	(func $start
		call $addTwo1
		drop
	)
	(start $start)
)
