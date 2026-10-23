;; Exporter: "mk" returns a (ref (struct (field i32))). The importer next to it
;; receives this object across the module boundary.
(module
  (type $s (struct (field i32)))
  (func (export "mk") (result (ref $s))
    i32.const 7 struct.new $s))
