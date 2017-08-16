(module
  (func $start
    (block (result i32)
			(block (result i32)
				(block (result i32)
					i32.const 4
					i32.const 1
					br_table 2 1 0
				)
				unreachable
				i32.const 2
			)
			unreachable
			i32.const 1
	  )
	  i32.const 0
    unreachable
  )
  (start $start)
)
