;; #39: imports a.mk and casts its result to this module's own type 0, which is
;; a DIFFERENT shape that happens to share the index. The object is really a
;; { i32 } from module a, so the cast must trap -- not succeed because both
;; modules numbered a struct 0.
(module
  (type $s2 (struct (field i64) (field i64)))
  (type $ft (func (result (ref 0))))
  (import "a" "mk" (func $mk (type $ft)))
  (func $start
    call $mk
    ref.cast (ref 0)
    drop)
  (start $start))
