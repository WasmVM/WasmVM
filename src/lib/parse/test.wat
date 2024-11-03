(module
    (type (func))
    (func (export "func0") (import "testmod" "funcA") (type 0))
    (func (export "func1") (type 0) (local i32))
    (func (export "func2") (export "func3") (type 0) (local i64))
    (func $func2 (export "func4") (type 0) (local f32))
)