(module
    (table $t0 1 2 funcref)
    (memory $m0 3 4)
    (global $g0 i64 i64.const 5)
    (export "table0" (table 0))
    (export "table1" (table $t0))
    (export "mem0" (memory 0))
    (export "mem1" (memory $m0))
    (export "glo0" (global 0))
    (export "glo1" (global $g0))
    ;; TODO: func
)