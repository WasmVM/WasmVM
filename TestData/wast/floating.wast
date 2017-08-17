(module
  (memory (data "\00\00\00\00\F0\F0\F0\FF\00\00\00\00\F0\F0\F0\7F\00\00\00\00\00\00\F0\FF\00\00\00\00\00\00\F0\7F\00\00\00\00\00\00\00\80\00\00\00\00\00\00\00\00"))
  ;; Offset 0:-nan 8:+nan 16:-inf 24:+inf 32:-0 40:+0
  (func $start
	i32.const 0
	f64.load offset=0 ;;-nan
	f64.const 2.5
	f64.eq
  	i32.const 0
	f64.load offset=8 ;;+nan
	f64.const 2.5
	f64.eq
	unreachable
	f64.const 2.5
	i32.const 0
	f64.load offset=0 ;;-nan
	f64.eq
	f64.const 2.5
	i32.const 0
	f64.load offset=8 ;;+nan
	f64.eq
	unreachable
	i32.const 0
	f64.load offset=24 ;;+inf
	f64.const 2.5
	f64.eq
	unreachable
	i32.const 0
	f64.load offset=16 ;;-inf
	f64.const 2.5
	f64.eq
	unreachable
	f64.const 2.5
	i32.const 0
	f64.load offset=24 ;;+inf
	f64.eq
	unreachable
	f64.const 2.5
	i32.const 0
	f64.load offset=16 ;;-inf
	f64.eq
	unreachable
	i32.const 0
	f64.load offset=40 ;;+0
	i32.const 0
	f64.load offset=32 ;;-0
	f64.eq
	i32.const 0
	f64.load offset=32 ;;-0
	i32.const 0
	f64.load offset=40 ;;+0
	f64.eq
	unreachable
	f64.const 2.5
	f64.const 5.0
	f64.eq			;; <
	unreachable
	f64.const 3.14
	f64.const 3.14
	f64.eq			;; =
	unreachable
	f64.const 7.53
	f64.const 5.0
	f64.eq			;; >
	unreachable
  )
  (start $start)
)
