;; #39 across a module boundary via a GLOBAL import: imports a."g" (a global
;; holding a's {i32} struct) and casts it to this module's type 0, {i64,i64}.
;; Global-import type matching is still structural index comparison (canonical
;; import matching for globals is later work), so the foreign object flows in
;; and the ref.cast must trap on the Store-wide type identity -- demonstrating
;; the cross-module struct cast check.
(module
  (type $s2 (struct (field i64) (field i64)))
  (global $g (import "a" "g") (mut (ref null 0)))
  (func $start
    global.get $g
    ref.cast (ref 0)
    drop)
  (start $start))
