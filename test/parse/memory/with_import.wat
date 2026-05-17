(module
    (memory (import "testmod" "mem0") 12)
    (memory $mem1 (import "testmod" "mem1") 13)
    (memory 14)
    (memory (import "testmod" "mem3") 15)
)