(module
    (memory (export "mem0") (import "testmod" "memA") 6)
    (memory (export "mem1") 6 10)
    (memory (export "mem2") (export "mem3") 8 12)
    (memory $mem2 (export "mem4") 7)
)