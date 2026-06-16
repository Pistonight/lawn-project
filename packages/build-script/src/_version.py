import subprocess
from datetime import datetime
from pathlib import Path

from . import _common

def generate_buildinfo_inc(input_version_file: Path | str, output_inc_file: Path | str):
    version = Path(input_version_file).read_text().strip()
    print("==> version: " + version)
    output_inc_file = Path(output_inc_file)
    # not making parent because it should exist
    repo_root = _common.get_root_root();
    git_commit = subprocess.check_output(["git", "rev-parse", "HEAD"], cwd=repo_root, text=True).strip()
    print("==> commit:  " + git_commit)

    major, minor, patch = [x.strip() for x in version.split('.')]

    # generate ResoddedFramework-compatible BuildInfo.h
    defines = {
        "GIT_HASH": '"' + git_commit + '"',
        "GIT_IS_DIRTY": "false",
        "GIT_AVAILABLE": "true",
        "BUILD_NUMBER": "2", # 2 = release
        "VERSION_MAJOR": major,
        "VERSION_MINOR": minor,
        "VERSION_PATCH": patch,
        "BUILD_DATE": '"' + datetime.now().strftime("%Y/%m/%d %H:%M:%S") + '"',
    }

    lines = [
        "// clang-format off", 
    ] + [
        f"#define {name} {value}" for name, value in defines.items()
    ]
    content = "\n".join(lines) + "\n"

    output_inc_file.write_text(content)
