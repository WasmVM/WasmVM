(module
    (type $t0 (func))
    (func 
        (unreachable)
        (unreachable (nop (return)))
        (block nop)
        (loop unreachable)
        (if (then unreachable))
        (if (then unreachable) (else nop))
        (if (return) (then unreachable) (else nop))
    )
)