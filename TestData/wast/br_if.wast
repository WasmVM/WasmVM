(module
	(func $br
		(loop
			i32.const 63
			unreachable
			i32.const 1
			br_if 1
			i32.const 62
			unreachable
		)
	)
	(func $start
		call $br
	)
	(start $start)
)
