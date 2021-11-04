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
    )
    # Load module
    case_file.write(
        '  // Load\n'
        '  size_t bin_size = 0;\n'
        f'  byte_t* bin_data = load_file("{wasm_file}", &bin_size);\n'
        '  if(bin_data == NULL){\n'
        f'    fprintf(stderr, "{wasm_file}({wast_line}): [Error] cannot load module\\n");\n'
        '    result += 1;\n'
        '    failed = 1;\n'
        '  }\n'
    )
    # Decode module
    case_file.write(
        '  // Decode\n'
        '  module_free(module);\n'
        '  module = NULL;\n'
        '  if(!failed){\n'
        '    if(module_decode(bin_data, bin_size, &module)){\n'
        f'      fprintf(stderr, "{wasm_file}({wast_line}): [Failed] failed decoding module with error \'%s\'\\n",  wasmvm_strerror(wasmvm_errno));\n'
        '      result += 1;\n'
        '      failed = 1;\n'
        '    }\n'
        '    free(bin_data);\n'
        '  }\n'
    )
    # Import matching
    case_file.write(
        '  // Get import\n'
        '  ExternVal* externVals = NULL;\n'
        '  size_t externSize = 0;\n'
        '  if(!failed){\n'
        '    if(match_imports(module, moduleInsts, &externSize, &externVals)){\n'
        f'      fprintf(stderr, "{wasm_file}({wast_line}): [Failed] failed match imports with error \'%s\'\\n",  wasmvm_strerror(wasmvm_errno));\n'
        '      result += 1;\n'
        '      failed = 1;\n'
        '    }\n'
        '  }\n'
    )
    # Instantiate module
    case_file.write(
        '  // Instantiate\n'
        '  module_inst_free(module_inst);\n'
        '  module_inst = NULL;\n'
        '  if(!failed){\n'
        '    if(module_instantiate(store, module, externSize, externVals, &module_inst)){\n'
        f'      fprintf(stderr, "{wasm_file}({wast_line}): [Failed] failed instantiate module with error \'%s\'\\n",  wasmvm_strerror(wasmvm_errno));\n'
        '      result += 1;\n'
        '      failed = 1;\n'
        '    }\n'
        '  }\n'
    )
    # End
    case_file.write(
        '  // End\n'
        '  if(!failed){\n'
        f'    fprintf(stderr, "{wasm_file}({wast_line}): [Passed]\\n");\n'
        '  }\n'
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
        )
        # Load module
        case_file.write(
            '  // Load\n'
            '  size_t bin_size = 0;\n'
            f'  byte_t* bin_data = load_file("{wasm_file}", &bin_size);\n'
            '  if(bin_data == NULL){\n'
            f'    fprintf(stderr, "{wasm_file}({wast_line}): [Error] cannot load module\\n");\n'
            '    result += 1;\n'
            '    failed = 1;\n'
            '  }\n'
        )
        # Decode module
        case_file.write(
            '  // Decode\n'
            '  wasm_module malformed = NULL;\n'
            '  if(!failed){\n'
            '    if(module_decode(bin_data, bin_size, &malformed) == ERROR_success){\n'
            f'      fprintf(stderr, "{wasm_file}({wast_line}): [Failed] should be malformed\\n");\n'
            '      result += 1;\n'
            '      failed = 1;\n'
            '    }\n'
            '    free(bin_data);\n'
            '  }\n'
        )
        # End
        case_file.write(
            '  // End\n'
            '  if(!failed){\n'
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

def generate_case_main(case_name: str, case_dir: Path, case_json: dict) -> None:
    with open(str(case_dir.joinpath(f"{case_name}.c")), "w") as case_file:
        # Prologue
        case_file.write(
            f"/*** spec test - {case_name} ***/\n"
            "#include <WasmVM.h>\n"
            "#include <helper.h>\n"
            "#include <hashmap_t.h>\n"
            "#include <stdio.h>\n"
            "#include <string.h>\n"
            "#include <stdlib.h>\n"
            "\n"
            "int main(void){\n"
            "int result = 0;\n"
            "hashmap_t(wasm_module_inst) moduleInsts = NULL;\n"
            "wasm_module module = NULL;\n"
            "wasm_module_inst module_inst = NULL;\n"
            "wasm_store store = store_init();\n"
            "\n"
        )
        # Commands
        for command in case_json["commands"]:
            if command["type"] == "module":
                action_module(case_file, command)
            elif command["type"] == "assert_malformed":
                action_assert_malformed(case_file, command)

        # Epilogue
        case_file.write(
            "// Clean\n"
            "store_free(store);\n"
            "module_free(module);\n"
            "module_inst_free(module_inst);\n"
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