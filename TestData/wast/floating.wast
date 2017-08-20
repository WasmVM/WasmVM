(module
  (memory (data "\F0\F0\FF\FF\F0\F0\FF\7F\00\00\80\FF\00\00\80\7F\00\00\00\80\00\00\00\00"))
  ;; Offset 0:-nan 8:+nan 16:-inf 24:+inf 32:-0 40:+0
  (func $start
  	;; demote
		i32.const 0  ;;-nan
		f32.load
		i32.reinterpret/f32
		i32.const 4  ;;nan
		f32.load
		i32.reinterpret/f32
		i32.const 8  ;;-inf
		f32.load
		i32.reinterpret/f32
		i32.const 12  ;;+inf
		f32.load
		i32.reinterpret/f32
		i32.const 16  ;;-0
		f32.load
		i32.reinterpret/f32
		i32.const 20  ;;+0
		f32.load
		i32.reinterpret/f32
		f32.const -12.8 ;;neg
		i32.reinterpret/f32
		f32.const 12.8 ;;pos
		i32.reinterpret/f32
		unreachable
		
  )
  (start $start)
)
