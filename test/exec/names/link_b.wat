(module $b
  (type $tb (func (param $pb i32)))
  (import "link_a.wasm" "f" (func $f_from_b (type $tb)))
  (import "env" "ext" (func $ext_b (type $tb)))
  (global $g i32 (i32.const 2))
  (func $start_b)
  (start $start_b)
)
