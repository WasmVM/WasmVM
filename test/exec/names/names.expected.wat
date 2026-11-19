(module $mod 
  (type $ft (func (param $fp i32) (result i32) ) )
  (type $st (struct (field $fa i32) (field $fb (mut i64)) ) )
  (type (func ) )
  (type (func (param i32) ) )
  (import "env" "imp" (func $imp (type $ft) ) )
  (import "env" "itag" (tag $itag (type 3) ) )
  (func $main (type $ft) (local $loc i32) (local $obj (ref null $st))
    block $out
    loop $in
    local.get 0
    br_if $out
    br $in
    end
    end
    local.get $obj
    struct.get $st $fa
    drop
    local.get $loc
    global.get 0
    i32.add
    call $imp
  )
  (table $tab 1 funcref)
  (memory $mem 1)
  (tag $tg (type 3) )
  (global $glob (mut i32) i32.const 7)
  (export "main" (func $main) )
  (elem $seg (table $tab) (offset i32.const 0) funcref 
    (item ref.func $main)
  )
  (data $dat (memory $mem) (offset i32.const 0) 
    "\78"
  )
)
