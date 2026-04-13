// Copyright 2025 Luis Hsu. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#ifndef WASMVM_HOST_SYS_FS_DEF
#define WASMVM_HOST_SYS_FS_DEF

// wasm_stat layout written into linear memory (56 bytes, no internal padding):
//
//  offset  size  type      field
//   0       4    uint32    dev
//   4       4    uint32    ino
//   8       4    uint32    mode
//  12       4    uint32    nlink
//  16       4    uint32    uid
//  20       4    uint32    gid
//  24       8    uint64    size
//  32       8    uint64    atime_ns   (nanoseconds since epoch)
//  40       8    uint64    mtime_ns
//  48       8    uint64    ctime_ns
// total: 56 bytes
#define WASM_STAT_SIZE 56

// wasm_dirent layout written into linear memory (260 bytes):
//
//  offset  size  type      field
//   0       4    uint32    ino
//   4       1    uint8     type  (DT_REG=8, DT_DIR=4, DT_LNK=10, DT_UNKNOWN=0)
//   5     255    char[]    name  (null-terminated, max 254 chars + null)
// total: 260 bytes
#define WASM_DIRENT_SIZE 260

#endif // WASMVM_HOST_SYS_FS_DEF
