(module
    (type $before (func))
    (rec
        (type $a (sub (struct (field i32))))
        (type $b (sub $c (struct (field i64))))
        (type $c (sub (struct (field i64))))
    )
    (rec (type $solo (array i8)))
    (type $after (func (param i32)))
)
