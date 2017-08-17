(module
  (func $start
	f32.const 2.5
	f32.const 2.5
	f32.eq
	unreachable
	f32.const 3.14
	f32.const 2.5
	f32.eq
	unreachable
	f32.const -3.14
	f32.const 2.5
	f32.eq
	unreachable
  )
  (start $start)
)
