(module $mod
  (type $ft (func (param $fp i32) (result i32)))
  (type $st (struct (field $fa i32) (field $fb (mut i64))))
  (import "env" "imp" (func $imp (type $ft)))
  (import "env" "itag" (tag $itag (param i32)))
  (table $tab 1 funcref)
  (memory $mem 1)
  (tag $tg (param $tgp i32))
  (global $glob (mut i32) (i32.const 7))
  (func $main (export "main") (param $arg i32) (result i32) (local $loc i32) (local $obj (ref null $st))
    block $out
      loop $in
        local.get $arg
        br_if $out
        br $in
      end
    end
    local.get $obj
    struct.get $st $fa
    drop
    local.get $loc
    global.get $glob
    i32.add
    call $imp)
  (elem $seg (table $tab) (i32.const 0) func $main)
  (data $dat (memory $mem) (i32.const 0) "x")
)
