from sys import argv
from pathlib import Path, PurePath
import subprocess
import json
from typing import TextIO

def action_module(case_file: TextIO, command: dict) -> None:
    # Get wasm file name
    wasm_file = command["filename"]
    # Decode module
    case_file.write(
        f'/** module "{wasm_file}" */\n'
        '{  // Load module\n'
        '  size_t bin_size = 0;\n'
        f'  byte_t* bin_data = load_file("{wasm_file}", &bin_size);\n'
        '  if(bin_data == NULL){\n'
        f'    fprintf(stderr, "error: cannot load module \'{wasm_file}\'\\n");\n'
        '    return -1;\n'
        '  }\n'
        '  wasm_module module = NULL;\n'
        '  if(module_decode(bin_data, bin_size, &module)){\n'
        f'    fprintf(stderr, "error: cannot decode module \'{wasm_file}\'\\n");\n'
        '    return -1;\n'
        '  }\n'
        '  module_free(module);\n'
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
            "#include <stdio.h>\n"
            "\n"
            "int main(void){\n"
            "int result = 0;\n"
            "\n"
        )
        # Commands
        for command in case_json["commands"]:
            if command["type"] == "module":
                action_module(case_file, command)

        # Epilogue
        case_file.write(
            "\n"
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