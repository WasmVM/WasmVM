(module
  (import "addTwo.wasm" "addTwo" (func (param i32 i32) (result i32)))
  (import "block.wasm" "start" (func))
  (memory (data "hi"))
  (type (func (param i32) (result i32)))
  (start 1)
  (table 0 1 anyfunc)
  (func)
  (func
    i32.const 420000000
    drop)
  (export "e" (func 1)))
