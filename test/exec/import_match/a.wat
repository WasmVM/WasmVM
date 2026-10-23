;; Exporter: "f" is a function whose declared type $g is a subtype of $t, with a
;; refined (result-covariant) signature. An importer that declares the import as
;; $t must accept it by subtyping, though the signatures are not identical.
(module
  (type $t (sub (func (result (ref null any)))))
  (type $g (sub $t (func (result (ref null eq)))))
  (func (export "f") (type $g) (ref.null eq)))
