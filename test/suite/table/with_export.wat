(module
    (table (export "table0") (import "testmod" "tableA") 6 10 funcref)
    (table (export "table1") 6 funcref)
    (table (export "table2") (export "table3") 8 20 externref)
    (table $table3 (export "table4") 7 funcref)
)