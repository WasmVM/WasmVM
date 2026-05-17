// Copyright 2025 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef WASMVM_HOST_SYS_PROC_DEF
#define WASMVM_HOST_SYS_PROC_DEF

// clock_gettime timespec layout written into linear memory (12 bytes):
//
//  offset  size  type    field
//   0       8    int64   tv_sec   (seconds since epoch)
//   8       4    int32   tv_nsec  (nanoseconds, 0–999999999)
// total: 12 bytes
#define WASM_TIMESPEC_SIZE 12

// clk_id values (mirror POSIX):
//   0 = CLOCK_REALTIME
//   1 = CLOCK_MONOTONIC
#define WASM_CLOCK_REALTIME  0
#define WASM_CLOCK_MONOTONIC 1

#endif // WASMVM_HOST_SYS_PROC_DEF
