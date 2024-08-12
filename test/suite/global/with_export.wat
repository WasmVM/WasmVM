(module
    (global (export "glo0") (import "testmod" "gloA") i32)
    (global (export "glo1") i64 i64.const 5)
    (global (export "glo2") (export "glo3") (mut f32) f32.const 2.8)
    (global $glo2 (export "glo4") funcref ref.null func)
)