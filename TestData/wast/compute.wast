(module
  (func $start
		i32.const 30
		f64.convert_s/i32
		i32.const -30
		f64.convert_s/i32
		unreachable
		i32.const 30
		f64.convert_u/i32
		i32.const -30
		f64.convert_u/i32
		unreachable
		i64.const 30
		f64.convert_s/i64
		i64.const -30
		f64.convert_s/i64
		unreachable
		i64.const 30
		f64.convert_u/i64
		i64.const -30
		f64.convert_u/i64
		unreachable
  )
  (start $start)
)
