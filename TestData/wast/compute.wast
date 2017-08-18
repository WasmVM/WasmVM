(module
  (func $start
			i32.const -12
			i32.const -5
			i32.rem_s
			i32.const -12
			i32.const -5
			i32.rem_u
			i32.const -12
			i32.const 5
			i32.rem_s
			i32.const -12
			i32.const 5
			i32.rem_u
			unreachable
  )
  (start $start)
)
