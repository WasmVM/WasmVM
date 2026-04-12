(module
    (type (func (param i32)))
    (tag $t0 (type 0))
    (func
        throw_ref
        throw $t0
        try_table (catch $t0 0)
            nop
        end
        try_table (catch_ref $t0 0) (catch_all 0)
            nop
        end
        try_table (catch_all_ref 0)
            nop
        end
    )
)
