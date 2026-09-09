(module
    (type $base (sub (struct (field i32))))
    (type $derived (sub $base (struct (field i32) (field i64))))
    (type $leaf (sub final $derived (struct (field i32) (field i64) (field f32))))
    (type $numbered (sub 0 (struct (field i32))))
    (type $plain (sub final (func)))
)
