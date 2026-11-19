(module 
  (type $tb (func (param $pb i32) ) )
  (type (func ) )
  (import "env" "ext" (func $ext_b (type $tb) ) )
  (func $start_b (type 1)
  )
  (func $f (type $tb) (local $la i32)
    block $blk
    local.get $la
    call $ext_b
    end
  )
  (func $start_a (type 1)
  )
  (func (type 1)
    call $start_b
    call $start_a
  )
  (global $g i32 i32.const 2)
  (global i32 i32.const 1)
  (start 4)
)
