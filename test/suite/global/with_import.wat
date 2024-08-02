(module
    (global (import "testmod" "glo0") externref)
    (global $glo1 (import "testmod" "glo1") f32)
    (global i32 i32.const 3)
    (global (import "testmod" "glo3") (mut f64))
)