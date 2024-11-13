(module
    (global i32 i32.const 1)
    (func (local $l0 i32) (local $l1 i32)
        local.get 0
        local.get $l1
        local.set 0
        local.tee 0
        global.get 0
        global.set 0
    )
)