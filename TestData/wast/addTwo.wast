(module 
	(func $addTwo (param i32 i32) (result i32)
		(i32.add 
			(get_local 0)
			(get_local 1)))
	(func $start
		i32.const -10000
		i32.const -20000
		i32.add
		drop
	)
	(start $start)
	(export "addTwo" (func $addTwo)))
