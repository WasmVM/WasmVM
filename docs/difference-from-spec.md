# Difference from spec

* No SIMD
* Allow multiple results in function & block
* Multiple memories
* Memory 64 as default
* While parsing text format, indices are assigned in the parse order. Imports are not preceded then other definitions. After converted to structure, indices will be fixed to the import-preceded order.
  - Ex:
    (func $fu1)
    (import "test" "testfunc" (func $fu2))
    (func $fu3)

    While parsing: $fu1 -> 0, $fu2 -> 1, $fu3 -> 2
    In structure: $fu1 -> 1, $fu2 -> 0, $fu3 -> 2