(module
	(func $br
		(loop
			i32.const 63
			drop
			br 1
			i32.const 62
			drop
		)
		i32.const 61
		drop
	)
	(func $start
		call $br
		i32.const 60
		drop
	)
	(start $start)
)
