(module 
	(global i32 (i32.const 300))
	(func $start
		get_global 0
		unreachable
		i32.const 500
		set_global 0
		get_global 0
		unreachable
	)
	(start $start)
)
