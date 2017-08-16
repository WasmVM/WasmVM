(module
	(func $br
		(loop
			i32.const 63
			unreachable
			br 1
			i32.const 62
			unreachable
		)
		i32.const 61
		unreachable
	)
	(func $start
		call $br
		i32.const 60
		unreachable
	)
	(start $start)
)
