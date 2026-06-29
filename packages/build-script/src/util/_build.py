import os
import shutil
import subprocess
import sys
import collections
from dataclasses import dataclass
from pathlib import Path
from threading import Thread
from typing import IO

from src.util import _fmt

@dataclass
class BuildArgs:
    is_release: bool   # build release flavor?
    is_raw: bool       # raw output?
    is_x86: bool       # x86 or x64?
    is_clean: bool     # clean output?
    is_configure: bool # force re-configure?
    do_ninja: bool     # re-configure ninja?
    is_all: bool       # clean or build all?
    is_help: bool      # HELP

def parse_args(argv: list[str]) -> BuildArgs:
    return BuildArgs(
        is_release="release" in argv or "--release" in argv,
        is_raw="--raw" in argv,
        is_x86="x86" in argv,
        is_clean="clean" in argv or "--clean" in argv,
        is_configure="configure" in argv or "--configure" in argv,
        do_ninja="ninja" in argv,
        is_all="all" in argv or "--all" in argv,
        is_help="-h" in argv or "--help" in argv or "help" in argv or "?" in argv,
    )

def print_arg_help():
    print("usage: build ARGS...")
    print("ARGS:")
    print("  release    build/configure release instead of debug")
    print("  x86 or x64 target x64 or x86 [default: x64]")
    print("  clean      clean build directories")
    print("  configure  force run cmake configure")
    print("  ninja      force run cmake configure for ninja")
    print("  all        clean/build all targets [clean all will stop after clean]")
    print("  --raw      print raw output")

def is_dir_configured(dir):
    build_cache = Path(dir) / "CMakeCache.txt"
    return build_cache.exists()

def is_ninja_dirty_in_build_dir(dir):
    build_cache = Path(dir) / ".ninja-dirty"
    return build_cache.exists()

def clear_ninja_dirty_in_build_dir(dir):
    build_cache = Path(dir) / ".ninja-dirty"
    build_cache.unlink(missing_ok=True)

def cmake_configure_ninja(project_dir, preset, build_dir, is_release, is_raw) -> int:
    if not is_dir_configured(build_dir):
        if Path(build_dir).exists():
            print("==> cleaning ninja configure dir")
            shutil.rmtree(build_dir)
    build_type = "Release" if is_release else "Debug"
    command = [
        "cmake", f"--preset={preset}", "-B", build_dir,
        "-G", "Ninja",
        "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON",
        f"-DCMAKE_BUILD_TYPE={build_type}"
    ]
    print(f"{_fmt.CYAN}==> configuring ninja ({build_type.lower()}){_fmt.RESET}")
    return _drive_cmake_configure(command, project_dir, is_raw)

def cmake_configure_msvc(project_dir, preset, build_dir, is_x86, is_release, is_raw) -> int:
    arch = "x86" if is_x86 else "x64"
    build_type = "Release" if is_release else "Debug"
    if not is_dir_configured(build_dir):
        if Path(build_dir).exists():
            print(f"==> cleaning msvc build ({arch},{build_type})")
            shutil.rmtree(build_dir)
    command = [
        "cmake", f"--preset={preset}", "-B", build_dir,
        "-G", "Visual Studio 18 2026",
    ]
    if is_x86:
        command += ["-A", "Win32"]
    else:
        command += ["-A", "x64"]
    print(f"{_fmt.CYAN}==> configuring msvc ({arch},{build_type}){_fmt.RESET}")
    return _drive_cmake_configure(command, project_dir, is_raw)

def _drive_cmake_configure(command, project_dir, is_raw) -> int:
    if is_raw:
        try:
            subprocess.check_call(command, cwd=project_dir)
            return 0
        except:
            print(f"{_fmt.RED}==> cmake configure failed{_fmt.RESET}")
            return 1

    status = _fmt.subprocess_call_rolling_window(command, cwd=project_dir);

    if status:
        print(f"{_fmt.CLEAR_LINE}==> {_fmt.RED}configure failed{_fmt.RESET}")
    else:
        print(f"{_fmt.CLEAR_LINE}==> cmake configure finished :)")

    return status

def cmake_build(build_dir: Path, is_release, is_raw) -> int:
    command = [
        "cmake", "--build", build_dir,
        "--config", "Release" if is_release else "Debug",
        "--",
        f"/m:{os.cpu_count()}"
    ]
    proc = subprocess.Popen(
        command,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
        bufsize=1
    )
    stderr = Thread(
        target=_drive_stderr,
        args=[proc.stderr]
    )
    if is_raw:
        stdout = Thread(
            target=_drive_stdout_raw,
            args=[proc.stdout, build_dir]
        )
    else:
        stdout = Thread(
            target=_drive_stdout,
            args=[proc.stdout, build_dir]
        )

    stdout.start()
    stderr.start()
    status = proc.wait()
    stdout.join()
    stderr.join()

    if status:
        print(f"{_fmt.CLEAR_LINE}{_fmt.RED}==> build failed D:{_fmt.RESET}")
    else:
        print(f"{_fmt.CLEAR_LINE}==> build successful :D")

    return status

def _drive_stderr(stream):
    for line in iter(stream.readline, ""):
        print(f"[err] {line}", end="")

def _drive_stdout_raw(stream, build_dir: Path):
    was_configured = False
    for line in iter(stream.readline, ""):
        if line.lstrip().startswith("CMake is re-running"):
            was_configured = True
        print(f"[out] {line}", end="")
    if was_configured:
        (build_dir / ".ninja-dirty").write_text("yes");

def _drive_stdout(stream: IO[str], build_dir: Path):
    was_configured = False
    is_in_cmake_configure = False
    width = shutil.get_terminal_size().columns
    for line in iter(stream.readline, ""):
        line_lstrip = line.lstrip()
        if line_lstrip.startswith("CMake is re-running"):
            is_in_cmake_configure = True
            was_configured = True
            print(f"{_fmt.CYAN}==> rerunning cmake config...{_fmt.RESET}{_fmt.YELLOW}")
            continue
        if "Build files have been written to" in line_lstrip:
            if is_in_cmake_configure:
                print(f"{_fmt.CLEAR_LINE}{_fmt.RESET}==> cmake reconfigure finished :)")
            is_in_cmake_configure = False
            continue
        if is_in_cmake_configure:
            # take out one char in case it goes out of screen
            l = line.rstrip()
            print(f"{_fmt.CLEAR_LINE}  {l[:width-3]}\r", end="", flush=True)
            continue

        
        if line_lstrip == line:
            line = line_lstrip.rstrip()
            is_error = "error" in line or "LNK" in line
            if is_error:
                line = _try_parse_compiler_error(line)
                print(f"{_fmt.CLEAR_LINE}{_fmt.RED}{line}{_fmt.RESET}")
                continue
            is_warning = "warning" in line
            if is_warning:
                print(f"{_fmt.CLEAR_LINE}{_fmt.YELLOW}{line}{_fmt.RESET}")
                continue
            print(f"{_fmt.CLEAR_LINE}{line}")

        else:
            line = line_lstrip.rstrip()
            if not line:
                print("")
                continue
            is_cc_progress = line.endswith(".cpp") or line.endswith(".c")
            if is_cc_progress:
                print(f"{_fmt.CLEAR_LINE}  Compile: {line}\r", end="", flush=True)
                continue
            if ".vcxproj -> " in line:
                _, artifact = line.split(".vcxproj -> ")
                print(f"{_fmt.CLEAR_LINE}Exported: {artifact}")
                continue

            print(f"{_fmt.CLEAR_LINE}. {line}")
    if was_configured:
        (build_dir / ".ninja-dirty").write_text("yes");

def _try_parse_compiler_error(line: str):
    if "): error C" not in line:
        return line
    # project location
    project = ""
    if line.endswith("]"):
        i = line.rfind("[")
        if i > 0:
            project = line[i:]
            line = line[:i].rstrip()
    file = ""
    i = line.find("):")
    if i > 0:
        file = line[:i+2]
        line = line[i+2:].lstrip()
    output = ""
    if file:
        output += file + "\n"
    output += _fmt.RESET + "  " + line + "\n"
    if project:
        output += "  " + project + "\n"
    return output.strip()


