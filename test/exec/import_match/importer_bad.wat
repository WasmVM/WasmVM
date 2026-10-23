;; Imports a."f" (exported type $g, result (ref null eq)) declared here as $u,
;; whose result (ref null extern) is in a different hierarchy. The export does
;; not equal, declare, or structurally refine $u, so the import is rejected.
(module
  (type $u (func (result (ref null extern))))
  (import "a" "f" (func (type $u))))
