(module
    (func $t0)
    (func
        ref.null func
        ref.null extern
        ref.is_null
        ref.func $t0
        ref.func 1
        ref.eq
        ref.as_non_null
        ref.i31
        i31.get_s
        i31.get_u
        ref.test (ref func)
        ref.test (ref null extern)
        ref.cast (ref func)
        ref.cast (ref null extern)
        any.convert_extern
        extern.convert_any
    )
)