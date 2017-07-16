(module
  (func $start
    block
    block
    block
    i32.const -1
    br_table 3 0 2 1 
    i32.const 3
    drop
    end
    i32.const 2
    drop
    end
    i32.const 1
    drop
    end
    i32.const 0
    drop
  )
  (start $start)
)
