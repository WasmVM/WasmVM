;; Control: this module's type 0 has the same shape as a's struct, so the two
;; roll to one identity and the same cast across the boundary must succeed
;; (instantiation completes, exit 0).
(module
  (type $s2 (struct (field i32)))
  (type $ft (func (result (ref 0))))
  (import "a" "mk" (func $mk (type $ft)))
  (func $start
    call $mk
    ref.cast (ref 0)
    drop)
  (start $start))
