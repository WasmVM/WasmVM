(module
  (memory $mem 1)
  (global $counter (mut i32) (i32.const 0))
  (global $limit i64 (i64.const -1))
  (func $add (param $a i32) (param $b i32) (result i32)
    local.get $a
    local.get $b
    i32.add
  )
  (func $main
    (local $x i32)
    i32.const 2
    i32.const 3
    call $add
    local.set $x
    block $done
      local.get $x
      global.set $counter
    end
  )
  (start $main)
)
