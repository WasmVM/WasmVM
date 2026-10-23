;; Exporter for the cross-module identity CLI tests.
;;  - "mk" returns a (ref $s), used by the isomorphic test.
;;  - "g" is a global holding a $s object (an {i32} struct), set by the start
;;    function; the mismatch test imports it and casts it to a differently
;;    shaped type 0.
(module
  (type $s (struct (field i32)))
  (func (export "mk") (result (ref $s)) i32.const 7 struct.new $s)
  (global $g (export "g") (mut (ref null $s)) (ref.null $s))
  (func $init i32.const 7 struct.new $s global.set $g)
  (start $init))
