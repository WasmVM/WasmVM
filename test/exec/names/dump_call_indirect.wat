(module
  (type $t (func))
  (type (func (param i32)))
  (table $tab 1 funcref)
  (func
    i32.const 0
    call_indirect $tab (type $t)
    i32.const 1
    i32.const 0
    call_indirect (type 1))
  (func
    i32.const 0
    return_call_indirect $tab (type $t))
  (func
    i32.const 1
    i32.const 0
    return_call_indirect (type 1))
)
