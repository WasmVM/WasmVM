(module
    (type (func))
    (type $ty1 (func (param i32)))
    (import "testmod" "func0" (func))
    (import "testmod" "func1" (func (type 0)))
    (import "testmod" "func2" (func (type $ty1)))
    (import "testmod" "func3" (func (type $ty1) (param f32)))
    (import "testmod" "func4" (func (type $ty1) (result f64)))
)