(module
    (type $empty (struct))
    (type $point (struct (field $x i32) (field $y i32)))
    (type $mixed (struct (field i8 (mut i16)) (field $r (mut f64)) (field funcref)))
)
