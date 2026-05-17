(module
    (type (func))
    (func
        call_ref 0
        return_call_ref 0
        ref.null func
        br_on_null 0
        ref.null func
        br_on_non_null 0
        ref.null func
        br_on_cast 0 (ref null func) (ref func)
        drop
        ref.null func
        br_on_cast_fail 0 (ref null func) (ref func)
        drop
    )
)
