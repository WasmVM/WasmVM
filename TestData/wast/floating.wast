(module
  (memory (data "\F0\F0\F0\FF\F0\F0\F0\7F\00\00\80\FF\00\00\80\7F\00\00\00\80\00\00\00\00"))
  ;; Offset 0:-nan 4:+nan 8:-inf 12:+inf 16:-0 20:+0
  (func $start
  	;; copysign
		f32.const 3.14 ;;pos
		i32.const 0  ;;-nan
		f32.load
		f32.copysign
		f32.const 3.14 ;;pos
		i32.const 4  ;;nan
		f32.load
		f32.copysign
		i32.const 0  ;;-nan
		f32.load
		f32.const 3.14 ;;pos
		f32.copysign
		i32.const 4  ;;nan
		f32.load
		f32.const 3.14 ;;pos
		f32.copysign
		i32.const 16  ;;-0
		f32.load
		i32.const 8  ;;-inf
		f32.load
		f32.copysign
		i32.const 20  ;;+0
		f32.load
		i32.const 12  ;;+inf
		f32.load
		f32.copysign
		i32.const 20  ;;+0
		f32.load
		i32.const 8  ;;-inf
		f32.load
		f32.copysign
		i32.const 16  ;;-0
		f32.load
		i32.const 12  ;;+inf
		f32.load
		f32.copysign
		i32.const 8  ;;-inf
		f32.load
		i32.const 16  ;;-0
		f32.load
		f32.copysign
		i32.const 12  ;;+inf
		f32.load
		i32.const 20  ;;+0
		f32.load
		f32.copysign
		i32.const 8  ;;-inf
		f32.load
		i32.const 20  ;;+0
		f32.load
		f32.copysign
		i32.const 12  ;;+inf
		f32.load
		i32.const 16  ;;-0
		f32.load
		f32.copysign
		i32.const 8  ;;-inf
		f32.load
		i32.const 8  ;;-inf
		f32.load
		f32.copysign
		i32.const 12  ;;+inf
		f32.load
		i32.const 12  ;;+inf
		f32.load
		f32.copysign
		i32.const 8  ;;-inf
		f32.load
		i32.const 12  ;;+inf
		f32.load
		f32.copysign
		i32.const 12  ;;+inf
		f32.load
		i32.const 8  ;;-inf
		f32.load
		f32.copysign
		f32.const 3.14 ;;pos
		i32.const 8  ;;-inf
		f32.load
		f32.copysign
		f32.const 3.14 ;;pos
		i32.const 12  ;;+inf
		f32.load
		f32.copysign
		i32.const 8  ;;-inf
		f32.load
		f32.const 3.14 ;;pos
		f32.copysign
		i32.const 12  ;;+inf
		f32.load
		f32.const 3.14 ;;pos
		f32.copysign
		i32.const 16  ;;-0
		f32.load
		i32.const 16  ;;-0
		f32.load
		f32.copysign
		i32.const 20  ;;+0
		f32.load
		i32.const 20  ;;+0
		f32.load
		f32.copysign
		i32.const 16  ;;-0
		f32.load
		i32.const 20  ;;+0
		f32.load
		f32.copysign
		i32.const 20  ;;+0
		f32.load
		i32.const 16  ;;-0
		f32.load
		f32.copysign
		f32.const 3.14 ;;pos
		i32.const 16  ;;-0
		f32.load
		f32.copysign
		f32.const 3.14 ;;pos
		i32.const 20  ;;+0
		f32.load
		f32.copysign
		i32.const 16  ;;-0
		f32.load
		f32.const 3.14 ;;pos
		f32.copysign
		i32.const 20  ;;+0
		f32.load
		f32.const 3.14 ;;pos
		f32.copysign
		f32.const 12 ;;pos
		f32.const 4 ;;pos
		f32.copysign
		f32.const -12 ;;neg
		f32.const -4 ;;neg
		f32.copysign
		f32.const 12 ;;pos
		f32.const -4 ;;neg
		f32.copysign
		f32.const -12 ;;neg
		f32.const 4 ;;pos
		f32.copysign
		unreachable
		
  )
  (start $start)
)
