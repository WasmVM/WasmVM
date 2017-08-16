(module
	(memory 2)
  (func $start
  	i32.const -3
  	grow_memory
  	current_memory
  	unreachable
  )
  (start $start)
)
