(module
    (type $t0 (func))
    (func 
        block $b0 
        nop
        end $b0

        block $b1 (result i32)
        nop
        end

        block (type 0)
        nop
        end

        loop
        nop
        end

        if $i0
        unreachable
        else $i0
        nop
        end $i0

        if $i1
        unreachable
        end

        loop
            block
            unreachable
            end
            block
            nop
            end
        end
    )
)