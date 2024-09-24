(module
    (type (func))
    (func 
        unreachable
        nop
        return
        br 1
        br_if 2
        br_table 3 4 5
        call 6
        call_indirect 7 (type 8)
        call_indirect (type 9)
    )
)