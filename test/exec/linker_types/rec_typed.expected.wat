(module 
  (type $a (func ) )
  (rec
    (type $b (func (param i32) ) )
  )
  (import "env" "g" (func (type $b) ) )
  (import "env" "t" (tag (type $b) ) )
  (func (type $b)
  )
)
