(module
    (memory 1 1)
    (func 
        i32.load offset=4 align=2
        i32.load8_s 1
        i32.load8_u 
        i32.load16_s 
        i32.load16_u 
        i32.store 
        i32.store8 
        i32.store16
        i64.load 
        i64.load8_s 
        i64.load8_u 
        i64.load16_s  
        i64.load16_u  
        i64.load32_s  
        i64.load32_u
        i64.store 
        i64.store8 
        i64.store16
        i64.store32
        f32.load 
        f32.store 
        f64.load 
        f64.store 
        memory.size
        memory.grow
        memory.fill
        memory.copy
        memory.init 1
        data.drop 2
    )
)