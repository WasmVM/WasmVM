;; Imports a."f" (exported type $g, result (ref null eq)) declared here as $h,
;; whose result is (ref null any). $g is NOT a declared subtype of $h, but it
;; refines it structurally (eq <: any, result-covariant), so the import rule
;; accepts it. Declared-only matching would reject this; the structural rule
;; accepts it.
(module
  (type $h (func (result (ref null any))))
  (import "a" "f" (func (type $h))))
