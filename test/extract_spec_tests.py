from functools import reduce
from sys import argv
from pathlib import Path, PurePath
import subprocess
import json
from typing import TextIO

def action_module(case_file: TextIO, command: dict) -> None:
    # Get wasm file name
    wasm_file = command["filename"]
    wast_line = command["line"]
    # Begin
    case_file.write(
        f'/** {wast_line}: module "{wasm_file}" */\n'
        '{\n'
        '  _Bool failed = 0;\n'
        '  wasmvm_errno = ERROR_success;\n'
    )
    # Load module
    case_file.write(
        '  // Load\n'
        f'  bytes_vector_t bin_data = load_file("{wasm_file}");\n'
        '  if(bin_data.data == NULL){\n'
        f'    fprintf(stderr, "{wasm_file}({wast_line}): [Error] cannot load module\\n");\n'
        '    failed = 1;\n'
        '  }\n'
    )
    # Decode module
    case_file.write(
        '  // Decode\n'
        '  module_free(module);\n'
        '  module = NULL;\n'
        '  if(!failed){\n'
        '    module = module_decode(bin_data);\n'
        '    if(wasmvm_errno != ERROR_success){\n'
        f'      fprintf(stderr, "{wasm_file}({wast_line}): [Failed] failed decoding module with error \'%s\'\\n",  wasmvm_strerror(wasmvm_errno));\n'
        '      failed = 1;\n'
        '    }\n'
        '    free_vector(bin_data);\n'
        '  }\n'
    )
    # Import matching
    case_file.write(
        '  // Get import\n'
        '  externval_vector_t externVals;\n'
        '  if(!failed){\n'
        '    externVals = match_imports(module, moduleInsts);\n'
        '    if(wasmvm_errno != ERROR_success){\n'
        f'      fprintf(stderr, "{wasm_file}({wast_line}): [Failed] failed match imports with error \'%s\'\\n",  wasmvm_strerror(wasmvm_errno));\n'
        '      failed = 1;\n'
        '      free_vector(externVals);\n'
        '    }\n'
        '  }\n'
    )
    # Instantiate module
    case_file.write(
        '  // Instantiate\n'
        '  module_inst_free(module_inst);\n'
        '  module_inst = NULL;\n'
        '  if(!failed){\n'
        '    module_inst = module_instantiate(store, module, externVals);\n'
        '    if(wasmvm_errno != ERROR_success){\n'
        f'      fprintf(stderr, "{wasm_file}({wast_line}): [Failed] failed instantiate module with error \'%s\'\\n",  wasmvm_strerror(wasmvm_errno));\n'
        '      failed = 1;\n'
        '    }\n'
        '  }\n'
    )
    # Named module
    if "name" in command:
        module_name = command["name"]
        case_file.write(
            '  if(!failed){\n'
            f'    hashmap_set(sizeof(char) * {len(module_name)}, \"{module_name}\", module_inst, named_modules);\n'
            '    last_regist_module_inst = module_inst;\n'
            '    module_inst = NULL;\n'
            '  }\n'
        )
    # End
    case_file.write(
        '  // End\n'
        '  if(failed){\n'
        '    result += 1;\n'
        '  }else{\n'
        f'    fprintf(stderr, "{wasm_file}({wast_line}): [Passed]\\n");\n'
        '  }\n'
        '  free_vector(externVals);\n'
        '}\n'
    )

def action_assert_malformed(case_file: TextIO, command: dict) -> None:
    if command["module_type"] == "binary":
        # Get info
        wasm_file = command["filename"]
        wast_line = command["line"]
        expected_text = command["text"]
        # Begin
        case_file.write(
            f'/** {wast_line}: assert_malformed module: {wasm_file} text: "{expected_text}"*/\n'
            '{\n'
            '  _Bool failed = 0;\n'
            '  wasmvm_errno = ERROR_success;\n'
        )
        # Load module
        case_file.write(
            '  // Load\n'
            '  size_t bin_size = 0;\n'
            f'  bytes_vector_t bin_data = load_file("{wasm_file}");\n'
            '  if(bin_data.data == NULL){\n'
            f'    fprintf(stderr, "{wasm_file}({wast_line}): [Error] cannot load module\\n");\n'
            '    failed = 1;\n'
            '  }\n'
        )
        # Decode module
        case_file.write(
            '  // Decode\n'
            '  wasm_module malformed = NULL;\n'
            '  if(!failed){\n'
            '    malformed = module_decode(bin_data);\n'
            '    if(wasmvm_errno == ERROR_success){\n'
            f'      fprintf(stderr, "{wasm_file}({wast_line}): [Failed] should be malformed\\n");\n'
            '      failed = 1;\n'
            '    }\n'
            '    free_vector(bin_data);\n'
            '  }\n'
        )
        # End
        case_file.write(
            '  // End\n'
            '  if(failed){\n'
            '    result += 1;\n'
            '  }else{\n'
            f'    if(strcmp(wasmvm_strerror(wasmvm_errno), "{expected_text}")){{\n'
            f'      fprintf(stderr, "{wasm_file}({wast_line}): [Failed] module_decode expected message \'{expected_text}\', but got \'%s\'\\n", wasmvm_strerror(wasmvm_errno));\n'
            '      result += 1;\n'
            '    }else{\n'
            f'      fprintf(stderr, "{wasm_file}({wast_line}): [Passed]\\n");\n'
            '    }\n'
            '  }\n'
            '  module_free(malformed);\n'
            '}\n'
            '\n'
        )

def action_register(case_file: TextIO, command: dict) -> None:
    # Get wasm file name
    module_as = command["as"]
    wast_line = command["line"]
    case_file.write(
        f'/** {wast_line}: register "{module_as}" */\n'
        '{\n'
    )
    if "name" in command:
        module_name = command["name"]
        case_file.write(
            '  wasm_module_inst regist_inst;\n'
            f'  hashmap_get(sizeof(char) * {len(module_name)}, \"{module_name}\", regist_inst, named_modules);\n'
            f'  hashmap_set(sizeof(char) * {len(module_as)}, \"{module_as}\", regist_inst, moduleInsts);\n'
        )
    else:
        case_file.write(
            f'  hashmap_set(sizeof(char) * {len(module_as)}, \"{module_as}\", module_inst, named_modules);\n'
            f'  hashmap_set(sizeof(char) * {len(module_as)}, \"{module_as}\", module_inst, moduleInsts);\n'
            '  last_regist_module_inst = module_inst;\n'
            '  module_inst = NULL;\n'
        )
    case_file.write(
        '}\n'
    )

def action_assert_return(case_file: TextIO, command: dict) -> None:
    wast_line = command["line"]
    action = command["action"]
    # Prologue
    module_name = (action["module"] if "module" in action else None)
    case_file.write(f'/** {wast_line}: assert_return {action["type"]} */\n')
    if module_name:
        case_file.write('{\n'
            '  _Bool failed = 0;\n'
            '  wasmvm_errno = ERROR_success;\n'
            '  wasm_module_inst module_inst_local = NULL;\n'
            f'  hashmap_get(sizeof(char) * {len(module_name)}, \"{module_name}\", module_inst_local, named_modules);\n'
        )
    else:
        case_file.write('{\n'
            '  _Bool failed = 0;\n'
            '  wasmvm_errno = ERROR_success;\n'
            '  wasm_module_inst module_inst_local = (module_inst) ? module_inst : last_regist_module_inst;\n'
        )
    if action["type"] == "invoke":
        # Props
        func_name = action["field"]
        func_args = action["args"]
        # Prepare args
        if(len(func_args) > 0): 
            case_file.write(
                '  // Func args\n'
                '  values_vector_t func_args;\n'
                f'  func_args.size = {len(func_args)};\n'
                f'  func_args.data = malloc(sizeof(wasm_value) * {len(func_args)});\n'
            )
            for arg_id, arg_val in enumerate(func_args):
                if arg_val["type"] == "i32":
                    case_file.write(
                        f'  func_args.data[{arg_id}].type = Value_i32;\n'
                        f'  func_args.data[{arg_id}].value.u32 = {arg_val["value"]}u;\n'
                    )
                elif arg_val["type"] == "i64":
                    case_file.write(
                        f'  func_args.data[{arg_id}].type = Value_i64;\n'
                        f'  func_args.data[{arg_id}].value.u64 = {arg_val["value"]}llu;\n'
                    )
                elif arg_val["type"] == "f32":
                    case_file.write(
                        f'  func_args.data[{arg_id}].type = Value_f32;\n'
                        f'  func_args.data[{arg_id}].value.u32 = {arg_val["value"]}u;\n'
                    )
                elif arg_val["type"] == "f64":
                    case_file.write(
                        f'  func_args.data[{arg_id}].type = Value_f64;\n'
                        f'  func_args.data[{arg_id}].value.u64 = {arg_val["value"]}llu;\n'
                    )
        else:
            case_file.write(
                '  // Func args\n'
                '  values_vector_t func_args;\n'
                '  vector_init(func_args);\n'
            )
        # Invoke
        sanitized_name = reduce(lambda res, elem: res + "," + hex(elem), func_name.encode(), "")[1:]
        name_size = len(func_name.encode())
        case_file.write(
            '  // Invoke\n'
            f'  byte_t func_name[{name_size}] = {{{sanitized_name}}};\n'
            f'  wasm_externval func_export = instance_export(module_inst_local, {name_size}, func_name);'
            '  if(wasmvm_errno != ERROR_success){\n'
            f'    fprintf(stderr, "assert_return({wast_line}): [Failed] failed retrieve export with error \'%s\'\\n",  wasmvm_strerror(wasmvm_errno));\n'
            '    failed = 1;\n'
            '  }\n'
            '  if(func_export.type != Desc_Func){\n'
            f'    fprintf(stderr, "assert_return({wast_line}): [Failed] export type is not function while invoke function\\n");\n'
            '    failed = 1;\n'
            '  }\n'
            '  if(!failed){\n'
            '    values_vector_t result = func_invoke(store, func_export.value, func_args);\n'
            '    if(wasmvm_errno != ERROR_success){\n'
            f'      fprintf(stderr, "assert_return({wast_line}): [Failed] failed invoke function with error \'%s\'\\n",  wasmvm_strerror(wasmvm_errno));\n'
            '      failed = 1;\n'
            '    }\n'
        )
        # Check expected
        func_expected = command["expected"]
        case_file.write(
            '    // Check\n'
            f'    if(result.size != {len(func_expected)}){{\n'
            f'      fprintf(stderr, "assert_return({wast_line}): [Failed] expected {len(func_expected)} results but we got %d\\n", result.size);\n'
            '      failed = 1;\n'
            '    }\n'
        )
        for exp_id, exp_val in enumerate(func_expected):
            if exp_val["type"] == "i32":
                case_file.write(
                    f'    if(!failed && ((result.data[{exp_id}].type != Value_i32) || (result.data[{exp_id}].value.u32 != {exp_val["value"]}u))){{\n'
                    f'      fprintf(stderr, "assert_return({wast_line}): [Failed] result[{exp_id}] (type: %d, value: %u) not match expected (type: %d, value: %u)\\n", result.data[{exp_id}].type, result.data[{exp_id}].value.u32, Value_i32, {exp_val["value"]}u);\n'
                    '      failed = 1;\n'
                    '    }\n'
                )
            elif exp_val["type"] == "i64":
                case_file.write(
                    f'    if(!failed && ((result.data[{exp_id}].type != Value_i64) || (result.data[{exp_id}].value.u64 != {exp_val["value"]}llu))){{\n'
                    f'      fprintf(stderr, "assert_return({wast_line}): [Failed] result[{exp_id}] (type: %d, value: %llu) not match expected (type: %d, value: %llu)\\n", result.data[{exp_id}].type, result.data[{exp_id}].value.u64, Value_i64, {exp_val["value"]}llu);\n'
                    '      failed = 1;\n'
                    '    }\n'
                )
            elif exp_val["type"] == "f32":
                if(exp_val["value"].startswith("nan")):
                    nan_val = exp_val["value"][4:]
                    if nan_val == "canonical":
                        case_file.write(
                            f'    if(!failed && ((result.data[{exp_id}].type != Value_f32) || !isnan(result.data[{exp_id}].value.f32) || ((result.data[{exp_id}].value.u32 & 0x7fffffffu) != 0x7fc00000u))){{\n'
                            f'      fprintf(stderr, "assert_return({wast_line}): [Failed] result[{exp_id}] (type: %d, value: %u) not match expected (type: %d, value: nan:canonical)\\n", result.data[{exp_id}].type, result.data[{exp_id}].value.u32, Value_f32);\n'
                            '      failed = 1;\n'
                            '    }\n'
                        )
                    elif nan_val == "arithmetic":
                        case_file.write(
                            f'    if(!failed && ((result.data[{exp_id}].type != Value_f32) || !isnan(result.data[{exp_id}].value.f32) || ((result.data[{exp_id}].value.u32 & 0x7fffffffu) < 0x7fc00000u) || ((result.data[{exp_id}].value.u32 & 0x7fffffffu) > 0x7fffffffu))){{\n'
                            f'      fprintf(stderr, "assert_return({wast_line}): [Failed] result[{exp_id}] (type: %d, value: %u) not match expected (type: %d, value: nan:arithmetic)\\n", result.data[{exp_id}].type, result.data[{exp_id}].value.u32, Value_f32);\n'
                            '      failed = 1;\n'
                            '    }\n'
                        )
                    else:
                        case_file.write(
                            f'    if(!failed && ((result.data[{exp_id}].type != Value_f32) || !isnan(result.data[{exp_id}].value.f32) || ((result.data[{exp_id}].value.u32 & 0x7fffffffu) != (0x7f800000u | {nan_val}u)))){{\n'
                            f'      fprintf(stderr, "assert_return({wast_line}): [Failed] result[{exp_id}] (type: %d, value: %u) not match expected (type: %d, value: %u)\\n", result.data[{exp_id}].type, result.data[{exp_id}].value.u32, Value_f32, (0x7f800000u | {nan_val}u));\n'
                            '      failed = 1;\n'
                            '    }\n'
                        )
                else:
                    case_file.write(
                        '    if(!failed){\n'
                        f'      u32_t diff = result.data[{exp_id}].value.u32 - {exp_val["value"]}u;\n'
                        f'      if(!failed && ((result.data[{exp_id}].type != Value_f32) || ((diff + 2) > 4))){{\n'
                        f'        fprintf(stderr, "assert_return({wast_line}): [Failed] result[{exp_id}] (type: %d, value: %u) not match expected (type: %d, value: %u)\\n", result.data[{exp_id}].type, result.data[{exp_id}].value.u32, Value_f32, {exp_val["value"]}u);\n'
                        '        failed = 1;\n'
                        '      }\n'
                        '    }\n'
                    )
            elif exp_val["type"] == "f64":
                if(exp_val["value"].startswith("nan")):
                    nan_val = exp_val["value"][4:]
                    if nan_val == "canonical":
                        case_file.write(
                            f'    if(!failed && ((result.data[{exp_id}].type != Value_f64) || !isnan(result.data[{exp_id}].value.f64) || ((result.data[{exp_id}].value.u64 & 0x7fffffffffffffffllu) != 0x7ff8000000000000llu))){{\n'
                            f'      fprintf(stderr, "assert_return({wast_line}): [Failed] result[{exp_id}] (type: %d, value: %llu) not match expected (type: %d, value: nan:canonical)\\n", result.data[{exp_id}].type, result.data[{exp_id}].value.u64, Value_f64);\n'
                            '      failed = 1;\n'
                            '    }\n'
                        )
                    elif nan_val == "arithmetic":
                        case_file.write(
                            f'    if(!failed && ((result.data[{exp_id}].type != Value_f64) || !isnan(result.data[{exp_id}].value.f64) || ((result.data[{exp_id}].value.u64 & 0x7fffffffffffffffllu) < 0x7ff8000000000000llu) || ((result.data[{exp_id}].value.u64 & 0x7fffffffffffffffllu) > 0x7fffffffffffffffllu))){{\n'
                            f'      fprintf(stderr, "assert_return({wast_line}): [Failed] result[{exp_id}] (type: %d, value: %llu) not match expected (type: %d, value: nan:arithmetic)\\n", result.data[{exp_id}].type, result.data[{exp_id}].value.u64, Value_f64);\n'
                            '      failed = 1;\n'
                            '    }\n'
                        )
                    else:
                        case_file.write(
                            f'    if(!failed && ((result.data[{exp_id}].type != Value_f64) || !isnan(result.data[{exp_id}].value.f64) || ((result.data[{exp_id}].value.u64 & 0x7fffffffffffffffllu) != (0x7ff8000000000000llu | {nan_val}llu)))){{\n'
                            f'      fprintf(stderr, "assert_return({wast_line}): [Failed] result[{exp_id}] (type: %d, value: %llu) not match expected (type: %d, value: %llu)\\n", result.data[{exp_id}].type, result.data[{exp_id}].value.u64, Value_f64, (0x7ff8000000000000llu | {nan_val}llu));\n'
                            '      failed = 1;\n'
                            '    }\n'
                        )
                else:
                    case_file.write(
                        '    if(!failed){\n'
                        f'      u64_t diff = result.data[{exp_id}].value.u64 - {exp_val["value"]}llu;\n'
                        f'      if(!failed && ((result.data[{exp_id}].type != Value_f64) || ((diff + 2) > 4))){{\n'
                        f'        fprintf(stderr, "assert_return({wast_line}): [Failed] result[{exp_id}] (type: %d, value: %llu) not match expected (type: %d, value: %llu)\\n", result.data[{exp_id}].type, result.data[{exp_id}].value.u64, Value_f64, {exp_val["value"]}llu);\n'
                        '        failed = 1;\n'
                        '      }\n'
                        '    }\n'
                    )

        # Clean
        case_file.write(
            '    free_vector(result);\n'
            '  }\n'
            '  // Clean\n'
            '  free_vector(func_args);\n'
        )
    elif action["type"] == "get":
        # Props
        field_name = action["field"]
        # Get
        sanitized_name = reduce(lambda res, elem: res + "," + hex(elem), field_name.encode(), "")[1:]
        name_size = len(field_name.encode())
        case_file.write(
            '  // Get\n'
            f'  byte_t export_name[{name_size}] = {{{sanitized_name}}};\n'
            f'  wasm_externval export = instance_export(module_inst_local, {name_size}, export_name);\n'
            '  if(wasmvm_errno != ERROR_success){\n'
            f'    fprintf(stderr, "assert_return({wast_line}): [Failed] failed retrieve export with error \'%s\'\\n",  wasmvm_strerror(wasmvm_errno));\n'
            '    failed = 1;\n'
            '  }\n'
            '  if(export.type != Desc_Global){\n'
            f'    fprintf(stderr, "assert_return({wast_line}): [Failed] export type is not global while get");\n'
            '    failed = 1;\n'
            '  }\n'
        )
        # Check expected
        case_file.write(
            '  // Check\n'
            '  if(!failed){\n'
            '    Value result = store->globals.data[export.value].val;\n'
        )
        exp_val = command["expected"][0]
        if exp_val["type"] == "i32":
            case_file.write(
                f'    if((result.type != Value_i32) || (result.value.u32 != {exp_val["value"]}u)){{\n'
                f'      fprintf(stderr, "assert_return({wast_line}): [Failed] result (type: %d, value: %u) not match expected (type: %d, value: %u)\\n", result.type, result.value.u32, Value_i32, {exp_val["value"]}u);\n'
                '      failed = 1;\n'
                '    }\n'
            )
        elif exp_val["type"] == "i64":
            case_file.write(
                f'    if((result.type != Value_i64) || (result.value.u64 != {exp_val["value"]}llu)){{\n'
                f'      fprintf(stderr, "assert_return({wast_line}): [Failed] result (type: %d, value: %llu) not match expected (type: %d, value: %llu)\\n", result.type, result.value.u64, Value_i64, {exp_val["value"]}llu);\n'
                '      failed = 1;\n'
                '    }\n'
            )
        elif exp_val["type"] == "f32":
            if(exp_val["value"].startswith("nan")):
                nan_val = exp_val["value"][4:]
                if nan_val == "canonical":
                    case_file.write(
                        f'    if((result.type != Value_f32) || !isnan(result.value.f32) || ((result.value.u32 & 0x7fffffffu) != 0x7fc00000u)){{\n'
                        f'      fprintf(stderr, "assert_return({wast_line}): [Failed] result (type: %d, value: %u) not match expected (type: %d, value: nan:canonical)\\n", result.type, result.value.u32, Value_f32);\n'
                        '      failed = 1;\n'
                        '    }\n'
                    )
                elif nan_val == "arithmetic":
                    case_file.write(
                        f'    if((result.type != Value_f32) || !isnan(result.value.f32) || ((result.value.u32 & 0x7fffffffu) < 0x7fc00000u) || ((result.value.u32 & 0x7fffffffu) > 0x7fffffffu)){{\n'
                        f'      fprintf(stderr, "assert_return({wast_line}): [Failed] result[{exp_id}] (type: %d, value: %u) not match expected (type: %d, value: nan:arithmetic)\\n", result.type, result.value.u32, Value_f32);\n'
                        '      failed = 1;\n'
                        '    }\n'
                    )
                else:
                    case_file.write(
                        f'    if((result.type != Value_f32) || !isnan(result.value.f32) || ((result.value.u32 & 0x7fffffffu) != (0x7f800000u | {nan_val}u))){{\n'
                        f'      fprintf(stderr, "assert_return({wast_line}): [Failed] result[{exp_id}] (type: %d, value: %u) not match expected (type: %d, value: %u)\\n", result.type, result.value.u32, Value_f32, (0x7f800000u | {nan_val}u));\n'
                        '      failed = 1;\n'
                        '    }\n'
                    )
            else:
                case_file.write(
                    '    {\n'
                    f'      u32_t diff = result.value.u32 - {exp_val["value"]}u;\n'
                    f'      if((result.type != Value_f32) || ((diff + 2) > 4)){{\n'
                    f'        fprintf(stderr, "assert_return({wast_line}): [Failed] result[{exp_id}] (type: %d, value: %u) not match expected (type: %d, value: %u)\\n", result.type, result.value.u32, Value_f32, {exp_val["value"]}u);\n'
                    '        failed = 1;\n'
                    '      }\n'
                    '    }\n'
                )
        elif exp_val["type"] == "f64":
            if(exp_val["value"].startswith("nan")):
                nan_val = exp_val["value"][4:]
                if nan_val == "canonical":
                    case_file.write(
                        f'    if((result.type != Value_f64) || !isnan(result.value.f64) || ((result.value.u64 & 0x7fffffffffffffffllu) != 0x7ff8000000000000llu)){{\n'
                        f'      fprintf(stderr, "assert_return({wast_line}): [Failed] result[{exp_id}] (type: %d, value: %llu) not match expected (type: %d, value: nan:canonical)\\n", result.type, result.value.u64, Value_f64);\n'
                        '      failed = 1;\n'
                        '    }\n'
                    )
                elif nan_val == "arithmetic":
                    case_file.write(
                        f'    if((result.type != Value_f64) || !isnan(result.value.f64) || ((result.value.u64 & 0x7fffffffffffffffllu) < 0x7ff8000000000000llu) || ((result.value.u64 & 0x7fffffffffffffffllu) > 0x7fffffffffffffffllu)){{\n'
                        f'      fprintf(stderr, "assert_return({wast_line}): [Failed] result[{exp_id}] (type: %d, value: %llu) not match expected (type: %d, value: nan:arithmetic)\\n", result.type, result.value.u64, Value_f64);\n'
                        '      failed = 1;\n'
                        '    }\n'
                    )
                else:
                    case_file.write(
                        f'    if((result.type != Value_f64) || !isnan(result.value.f64) || ((result.value.u64 & 0x7fffffffffffffffllu) != (0x7ff8000000000000llu | {nan_val}llu))){{\n'
                        f'      fprintf(stderr, "assert_return({wast_line}): [Failed] result[{exp_id}] (type: %d, value: %llu) not match expected (type: %d, value: %llu)\\n", result.type, result.value.u64, Value_f64, (0x7ff8000000000000llu | {nan_val}llu));\n'
                        '      failed = 1;\n'
                        '    }\n'
                    )
            else:
                case_file.write(
                    '    if(!failed){\n'
                    f'      u64_t diff = result.value.u64 - {exp_val["value"]}llu;\n'
                    f'      if((result.type != Value_f64) || ((diff + 2) > 4)){{\n'
                    f'        fprintf(stderr, "assert_return({wast_line}): [Failed] result[{exp_id}] (type: %d, value: %llu) not match expected (type: %d, value: %llu)\\n", result.type, result.value.u64, Value_f64, {exp_val["value"]}llu);\n'
                    '        failed = 1;\n'
                    '      }\n'
                    '    }\n'
                )
        case_file.write('  }\n')
    # Epilogue
    case_file.write(
        '  // End\n'
        '  if(failed){\n'
        '    result += 1;\n'
        '  }else{\n'
        f'    fprintf(stderr, "assert_return({wast_line}): [Passed]\\n");\n'
        '  }\n'
        '}\n'
    )

def action_assert_trap(case_file: TextIO, command: dict) -> None:
    wast_line = command["line"]
    action = command["action"]
    # Prologue
    module_name = (action["module"] if "module" in action else None)
    case_file.write(f'/** {wast_line}: assert_trap */\n')
    if module_name:
        case_file.write('{\n'
            '  _Bool failed = 0;\n'
            '  wasmvm_errno = ERROR_success;\n'
            '  wasm_module_inst module_inst_local = NULL;\n'
            f'  hashmap_get(sizeof(char) * {len(module_name)}, \"{module_name}\", module_inst_local, named_modules);\n'
        )
    else:
        case_file.write('{\n'
            '  _Bool failed = 0;\n'
            '  wasmvm_errno = ERROR_success;\n'
            '  wasm_module_inst module_inst_local = (module_inst) ? module_inst : last_regist_module_inst;\n'
        )
    # Get props
    func_name = action["field"]
    func_args = action["args"]
    # Prepare args
    if(len(func_args) > 0): 
        case_file.write(
            '  // Func args\n'
            '  values_vector_t func_args;\n'
            f'  func_args.size = {len(func_args)};\n'
            f'  func_args.data = malloc(sizeof(wasm_value) * {len(func_args)});\n'
        )
        for arg_id, arg_val in enumerate(func_args):
            if arg_val["type"] == "i32":
                case_file.write(
                    f'  func_args.data[{arg_id}].type = Value_i32;\n'
                    f'  func_args.data[{arg_id}].value.u32 = {arg_val["value"]}u;\n'
                )
            elif arg_val["type"] == "i64":
                case_file.write(
                    f'  func_args.data[{arg_id}].type = Value_i64;\n'
                    f'  func_args.data[{arg_id}].value.u64 = {arg_val["value"]}llu;\n'
                )
            elif arg_val["type"] == "f32":
                case_file.write(
                    f'  func_args.data[{arg_id}].type = Value_f32;\n'
                    f'  func_args.data[{arg_id}].value.u32 = {arg_val["value"]}u;\n'
                )
            elif arg_val["type"] == "f64":
                case_file.write(
                    f'  func_args.data[{arg_id}].type = Value_f64;\n'
                    f'  func_args.data[{arg_id}].value.u64 = {arg_val["value"]}llu;\n'
                )
    else:
        case_file.write(
            '  // Func args\n'
            '  values_vector_t func_args;\n'
            '  vector_init(func_args);\n'
        )
    # Invoke
    sanitized_name = reduce(lambda res, elem: res + "," + hex(elem), func_name.encode(), "")[1:]
    name_size = len(func_name.encode())
    case_file.write(
        '  // Invoke\n'
        f'  byte_t func_name[{name_size}] = {{{sanitized_name}}};\n'
        f'  wasm_externval func_export = instance_export(module_inst_local, {name_size}, func_name);\n'
        '  if(wasmvm_errno != ERROR_success){\n'
        f'    fprintf(stderr, "assert_trap({wast_line}): [Failed] failed retrieve export with error \'%s\'\\n",  wasmvm_strerror(wasmvm_errno));\n'
        '    failed = 1;\n'
        '  }\n'
        '  if(func_export.type != Desc_Func){\n'
        f'    fprintf(stderr, "assert_trap({wast_line}): [Failed] export type is not function while invoke function");\n'
        '    failed = 1;\n'
        '  }\n'
        '  if(!failed){\n'
        '    values_vector_t result = func_invoke(store, func_export.value, func_args);\n'
        '    if(wasmvm_errno == ERROR_success){\n'
        f'      fprintf(stderr, "assert_trap({wast_line}): [Failed] assert_trap should not exit success\\n");\n'
        '      failed = 1;\n'
        '    }\n'
    )
    # Epilogue
    case_file.write(
        '    free_vector(result);\n'
        '  }\n'
        '  // Clean\n'
        '  free_vector(func_args);\n'
        '  // End\n'
        '  if(failed){\n'
        '    result += 1;\n'
        '  }else{\n'
        f'    fprintf(stderr, "assert_trap({wast_line}): [Passed]\\n");\n'
        '  }\n'
        '}\n'
    )

def action_action(case_file: TextIO, command: dict) -> None:
    wast_line = command["line"]
    action = command["action"]
    if action["type"] == "invoke":
        # Get props
        func_name = action["field"]
        func_args = action["args"]
        case_file.write(
            f'/** {wast_line}: invoke */\n'
            '{\n'
            '  _Bool failed = 0;\n'
            '  wasmvm_errno = ERROR_success;\n'
            '  wasm_module_inst module_inst_local = ((module_inst) ? module_inst : last_regist_module_inst);\n'
        )
        # Prepare args
        if(len(func_args) > 0): 
            case_file.write(
                '  // Func args\n'
                '  values_vector_t func_args;\n'
                f'  func_args.size = {len(func_args)};\n'
                f'  func_args.data = malloc(sizeof(wasm_value) * {len(func_args)});\n'
            )
            for arg_id, arg_val in enumerate(func_args):
                if arg_val["type"] == "i32":
                    case_file.write(
                        f'  func_args.data[{arg_id}].type = Value_i32;\n'
                        f'  func_args.data[{arg_id}].value.u32 = {arg_val["value"]}u;\n'
                    )
                elif arg_val["type"] == "i64":
                    case_file.write(
                        f'  func_args.data[{arg_id}].type = Value_i64;\n'
                        f'  func_args.data[{arg_id}].value.u64 = {arg_val["value"]}llu;\n'
                    )
                elif arg_val["type"] == "f32":
                    case_file.write(
                        f'  func_args.data[{arg_id}].type = Value_f32;\n'
                        f'  func_args.data[{arg_id}].value.u32 = {arg_val["value"]}u;\n'
                    )
                elif arg_val["type"] == "f64":
                    case_file.write(
                        f'  func_args.data[{arg_id}].type = Value_f64;\n'
                        f'  func_args.data[{arg_id}].value.u64 = {arg_val["value"]}llu;\n'
                    )
        else:
            case_file.write(
                '  // Func args\n'
                '  values_vector_t func_args;\n'
                '  vector_init(func_args);\n'
            )
        # Invoke
        sanitized_name = reduce(lambda res, elem: res + "," + hex(elem), func_name.encode(), "")[1:]
        name_size = len(func_name.encode())
        case_file.write(
            '  // Invoke\n'
            f'  byte_t func_name[{name_size}] = {{{sanitized_name}}};\n'
            f'  wasm_externval func_export = instance_export(module_inst_local, {name_size}, func_name);'
            '  if(wasmvm_errno != ERROR_success){\n'
            f'    fprintf(stderr, "invoke({wast_line}): [Failed] failed retrieve export with error \'%s\'\\n",  wasmvm_strerror(wasmvm_errno));\n'
            '    failed = 1;\n'
            '  }\n'
            '  if(func_export.type != Desc_Func){\n'
            f'    fprintf(stderr, "invoke({wast_line}): [Failed] export type is not function while invoke function");\n'
            '    failed = 1;\n'
            '  }\n'
            '  if(!failed){\n'
            '    values_vector_t result = func_invoke(store, func_export.value, func_args);\n'
            '    if(wasmvm_errno != ERROR_success){\n'
            f'      fprintf(stderr, "invoke({wast_line}): [Failed] failed invoke function with error \'%s\'\\n",  wasmvm_strerror(wasmvm_errno));\n'
            '      failed = 1;\n'
            '    }\n'
            '    free_vector(result);\n'
        )

        # Epilogue
        case_file.write(
            '  }\n'
            '  // Clean\n'
            '  free_vector(func_args);\n'
            '  // End\n'
            '  if(failed){\n'
            '    result += 1;\n'
            '  }else{\n'
            f'    fprintf(stderr, "invoke({wast_line}): [Passed]\\n");\n'
            '  }\n'
            '}\n'
        )
    
def action_assert_uninstantiable(case_file: TextIO, command: dict) -> None:
    # Get wasm file name
    wasm_file = command["filename"]
    wast_line = command["line"]
    # Begin
    case_file.write(
        f'/** {wast_line}: action_assert_uninstantiable "{wasm_file}" */\n'
        '{\n'
        '  _Bool failed = 0;\n'
        '  wasmvm_errno = ERROR_success;\n'
    )
    # Load module
    case_file.write(
        '  // Load\n'
        f'  bytes_vector_t bin_data = load_file("{wasm_file}");\n'
        '  if(bin_data.data == NULL){\n'
        f'    fprintf(stderr, "{wasm_file}({wast_line})[uninstantiable]: [Error] cannot load module\\n");\n'
        '    failed = 1;\n'
        '  }\n'
    )
    # Decode module
    case_file.write(
        '  // Decode\n'
        '  wasm_module module_local = NULL;\n'
        '  if(!failed){\n'
        '    module_local = module_decode(bin_data);\n'
        '    if(wasmvm_errno != ERROR_success){\n'
        f'      fprintf(stderr, "{wasm_file}({wast_line})[uninstantiable]: [Failed] failed decoding module with error \'%s\'\\n",  wasmvm_strerror(wasmvm_errno));\n'
        '      failed = 1;\n'
        '    }\n'
        '    free_vector(bin_data);\n'
        '  }\n'
    )
    # Import matching
    case_file.write(
        '  // Get import\n'
        '  externval_vector_t externVals;\n'
        '  if(!failed){\n'
        '    externVals = match_imports(module_local, moduleInsts);\n'
        '    if(wasmvm_errno != ERROR_success){\n'
        f'      fprintf(stderr, "{wasm_file}({wast_line})[uninstantiable]: [Failed] failed match imports with error \'%s\'\\n",  wasmvm_strerror(wasmvm_errno));\n'
        '      failed = 1;\n'
        '      free_vector(externVals);\n'
        '    }\n'
        '  }\n'
    )
    # Instantiate module
    case_file.write(
        '  // Instantiate\n'
        '  wasm_module_inst module_inst_local = NULL;\n'
        '  if(!failed){\n'
        '    module_inst_local = module_instantiate(store, module_local, externVals);\n'
        '    if(wasmvm_errno == ERROR_success){\n'
        f'      fprintf(stderr, "{wasm_file}({wast_line})[uninstantiable]: [Failed] assert_uninstantiable should not success\\n");\n'
        '      failed = 1;\n'
        '    }\n'
        '  }\n'
    )
    # End
    case_file.write(
        '  // End\n'
        '  if(failed){\n'
        '    result += 1;\n'
        '  }else{\n'
        f'    fprintf(stderr, "{wasm_file}({wast_line})[uninstantiable]: [Passed]\\n");\n'
        '  }\n'
        "  module_free(module_local);\n"
        "  module_inst_free(module_inst_local);\n"
        '  free_vector(externVals);\n'
        '}\n'
    )
    
def generate_case_main(case_name: str, case_dir: Path, case_json: dict) -> None:
    with open(str(case_dir.joinpath(f"{case_name}.c")), "w") as case_file:
        # Prologue
        case_file.write(
            f"/*** spec test - {case_name} ***/\n"
            "#include <WasmVM.h>\n"
            "#include <helper.h>\n"
            "#include <stdio.h>\n"
            "#include <math.h>\n"
            "#include <string.h>\n"
            "#include <stdlib.h>\n"
            "#include <hashmap_t.h>\n"
            "#include <spectest/spectest.h>\n"
            "#include <utils.h>\n"
            "\n"
            "void (*dump_stack)(wasm_stack stack) = _dump_stack;\n"
            "void (*dump_frame)(wasm_stack frame) = _dump_frame;\n"
            "\n"
            "int main(void){\n"
            "int result = 0;\n"
            "hashmap_t(wasm_module_inst) moduleInsts = NULL;\n"
            "hashmap_t(wasm_module_inst) named_modules = NULL;\n"
            "wasm_module module = NULL;\n"
            "wasm_module_inst module_inst = NULL;\n"
            "wasm_module_inst last_regist_module_inst = NULL;\n"
            "wasm_store store = store_init();\n"
            "hashmap_set(sizeof(char) * 8, \"spectest\", spectest_instanciate(store), moduleInsts);\n"
            "\n"
        )
        
        # Commands
        for command in case_json["commands"]:
            if command["type"] == "module":
                action_module(case_file, command)
            elif command["type"] == "assert_malformed":
                action_assert_malformed(case_file, command)
            elif command["type"] == "register":
                action_register(case_file, command)
            elif command["type"] == "assert_return":
                action_assert_return(case_file, command)
            elif command["type"] == "assert_trap":
                action_assert_trap(case_file, command)
            elif command["type"] == "action":
                action_action(case_file, command)
            elif command["type"] == "assert_uninstantiable":
                action_assert_uninstantiable(case_file, command)

        # Epilogue
        case_file.write(
            "// Clean\n"
            "store_free(store);\n"
            "module_free(module);\n"
            "module_inst_free(module_inst);\n"
            "free_moduleInst_maps(named_modules, moduleInsts);\n"
            "return result;\n"
            "}\n"
        )

if __name__ == "__main__":
    output_dir = Path(argv[1])
    spec_case_dir = Path("spec")
    for wast_file in spec_case_dir.glob("*.wast"):
        # Prepare case dir
        case_name = PurePath(wast_file).stem
        case_dir = output_dir.joinpath("spec", case_name)
        case_dir.mkdir(exist_ok=True, parents=True)
        # Call wast2json
        wast_process = subprocess.run([argv[2], "--no-check", str(Path.cwd().joinpath(wast_file).resolve())],
            cwd=str(case_dir.resolve())
        )
        # Parse json
        case_json = json.loads(case_dir.joinpath(f"{case_name}.json").read_text())
        # Generate C source of case
        generate_case_main(case_name, case_dir, case_json)