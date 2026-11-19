(module $a
  (type $ta (func (param $pa i32)))
  (import "env" "ext" (func $ext_a (type $ta)))
  (global $g i32 (i32.const 1))
  (func $f (export "f") (type $ta) (local $la i32)
    block $blk
      local.get $la
      call $ext_a
    end)
  (func $start_a)
  (start $start_a)
)
