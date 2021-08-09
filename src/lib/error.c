/**
 * Copyright 2021 Luis Hsu, Kevin Cyu. All rights reserved.
 * Use of this source code is governed by a BSD-style
 * license that can be found in the LICENSE file.
 */

#include <error.h>

int wasmvm_errno = ERROR_success;

const char* wasmvm_strerror(int errnum)
{
    switch (errnum) {
        case ERROR_success:
            return "success";
        case ERROR_duplicate_func:
            return "duplicate func";
        case ERROR_invalid_conv_int:
            return "invalid conversion to integer";
        case ERROR_too_many_locals:
            return "too many locals";
        case ERROR_div_zero:
            return "integer divide by zero";
        case ERROR_multi_mem:
            return "multiple memories";
        case ERROR_import_after_mem:
            return "import after memory";
        case ERROR_unknown_func:
            return "unknown function";
        case ERROR_res_before_param:
            return "result before parameter";
        case ERROR_malform_sec_id:
            return "malformed section id";
        case ERROR_len_out_of_bound:
            return "length out of bounds";
        case ERROR_align_gt_natural:
            return "alignment must not be larger than natural";
        case ERROR_int_too_large:
            return "integer too large";
        case ERROR_unexpected_end:
            return "unexpected end";
        case ERROR_magic_not_detect:
            return "magic header not detected";
        case ERROR_zero_expected:
            return "zero flag expected";
        case ERROR_const_out_range:
            return "constant out of range";
        case ERROR_int_overflow:
            return "integer overflow";
        case ERROR_uninit_elem:
            return "uninitialized element";
        case ERROR_unknown_global_0:
            return "unknown global 0";
        case ERROR_call_stack_exhst:
            return "call stack exhausted";
        case ERROR_unknown_global:
            return "unknown global";
        case ERROR_unknown_mem_1:
            return "unknown memory 1";
        case ERROR_unknown_bin_ver:
            return "unknown binary version";
        case ERROR_uninit:
            return "uninitialized";
        case ERROR_unknown_import:
            return "unknown import";
        case ERROR_i32_const:
            return "i32 constant";
        case ERROR_alignment:
            return "alignment";
        case ERROR_import_aft_glob:
            return "import after global";
        case ERROR_dup_export:
            return "duplicate export name";
        case ERROR_multi_start:
            return "multiple start sections";
        case ERROR_unknown_global_1:
            return "unknown global 1";
        case ERROR_inconsist_func:
            return "function and code section have inconsistent lengths";
        case ERROR_req_const_expr:
            return "constant expression required";
        case ERROR_sect_size_mis:
            return "section size mismatch";
        case ERROR_malform_import:
            return "malformed import kind";
        case ERROR_indir_call_mis:
            return "indirect call type mismatch";
        case ERROR_unexpect_end:
            return "unexpected end of section or function";
        case ERROR_indir_call:
            return "indirect call";
        case ERROR_elem_seg_not_fit:
            return "elements segment does not fit";
        case ERROR_inline_func:
            return "inline function type";
        case ERROR_multi_table:
            return "multiple tables";
        case ERROR_unknown_operator:
            return "unknown operator";
        case ERROR_malform_utf8:
            return "malformed UTF-8 encoding";
        case ERROR_type_mis:
            return "type mismatch";
        case ERROR_incomp_import:
            return "incompatible import type";
        case ERROR_undefined:
            return "undefined";
        case ERROR_malform_mut:
            return "malformed mutability";
        case ERROR_dup_table:
            return "duplicate table";
        case ERROR_unknown_label:
            return "unknown label";
        case ERROR_mis_label:
            return "mismatching label";
        case ERROR_import_aft_table:
            return "import after table";
        case ERROR_immut_global:
            return "global is immutable";
        case ERROR_mem_acc_out_bound:
            return "out of bounds memory access";
        case ERROR_junk_aft_sect:
            return "junk after last section";
        case ERROR_unknown_local:
            return "unknown local";
        case ERROR_data_seg_not_fit:
            return "data segment does not fit";
        case ERROR_start_func:
            return "start function";
        case ERROR_unknown_type:
            return "unknown type";
        case ERROR_unexpected_token:
            return "unexpected token";
        case ERROR_mem_size_gt_4g:
            return "memory size must be at most 65536 pages (4GiB)";
        case ERROR_undefined_elem:
            return "undefined element";
        case ERROR_unknown_mem:
            return "unknown memory";
        case ERROR_unreachable:
            return "unreachable";
        case ERROR_min_gt_max:
            return "size minimum must not be greater than maximum";
        case ERROR_int_rep_too_long:
            return "integer representation too long";
        case ERROR_dup_mem:
            return "duplicate memory";
        case ERROR_import_aft_func:
            return "import after function";
        case ERROR_dup_global:
            return "duplicate global";
        case ERROR_dup_local:
            return "duplicate local";
        case ERROR_unknown_table:
            return "unknown table";
        case ERROR_i32_const_range:
            return "i32 constant out of range";
        default:
            return "unknown error";
    }
}
