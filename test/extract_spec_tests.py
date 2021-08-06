from sys import argv
from pathlib import Path, PurePath
import subprocess
import json

if __name__ == "__main__":
    output_dir = Path(argv[1])
    spec_case_dir = Path("spec")
    for wast_file in spec_case_dir.glob("*.wast"):
        case_name = PurePath(wast_file).stem
        case_dir = output_dir.joinpath("spec", case_name)
        case_dir.mkdir(exist_ok=True, parents=True)
        wast_process = subprocess.run(["wast2json", "--no-check", str(Path.cwd().joinpath(wast_file).resolve())],
            cwd=str(case_dir.resolve())
        )
        case_json = json.load(case_dir.joinpath(case_name + ".json").resolve())
        pass