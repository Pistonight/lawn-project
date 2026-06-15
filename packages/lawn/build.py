import os
import sys
import subprocess
import shutil
from pathlib import Path
from threading import Thread
from typing import IO

CLEAR_LINE = "\r\033[2K"
RED = "\033[31m"
YELLOW = "\033[33m"
RESET = "\033[0m"

def main():
    # build 
    #   : configure if needed, build debug
    # build -- clean
    #   : configure if needed, clean and build debug
    # build -- clean configure
    #   : re-configure ninja, clean and build debug
    # build -- configure
    #   : re-configure ninja, build debug
    # build -- configure release
    #   : re-configure ninja as release, build release
    is_release = "release" in sys.argv or "--release" in sys.argv
    is_raw = "--raw" in sys.argv
    is_clean = "clean" in sys.argv or "--clean" in sys.argv
    is_configure = "configure" in sys.argv or "--configure" in sys.argv

    cmake_config_type = "Release" if is_release else "Debug"

    configure_dir = Path("../../build").resolve()
    if is_clean and is_configure:
        if configure_dir.exists():
            print("==> cleaning configure dir")
            shutil.rmtree(configure_dir)

    if is_configure or not configure_dir.exists():
        print("==> configuring ninja for: " + cmake_config_type.lower())
        subprocess.check_call([
            "cmake", "--preset=windows-msvc-x64", "-B", configure_dir,
            "-G", "Ninja",
            "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON",
            f"-DCMAKE_BUILD_TYPE={cmake_config_type}"
        ])

    build_dir = Path("build-x64/release" if is_release else "build-x64/debug")
    if is_clean:
        if build_dir.exists():
            print("==> cleaning build dir")
            shutil.rmtree(build_dir)
    if is_configure or not build_dir.exists():
        print("==> configuring msvc for: " + cmake_config_type.lower())
        build_dir.mkdir(parents=True, exist_ok=True)
        subprocess.check_call([
            "cmake", "--preset=windows-msvc-x64", "-B", build_dir,
            "-G", "Visual Studio 18 2026",
            # "-A", "x64" # 64-bit , use Win32 for x86
        ])

    exit(drive_cmake(build_dir, cmake_config_type, is_raw))

def drive_cmake(build_dir, config_name, is_raw):
    command = [
        "cmake", "--build", build_dir,
        "--config", config_name,
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
        target=drive_stderr,
        args=[proc.stderr]
    )
    if is_raw:
        stdout = Thread(
            target=drive_stdout_raw,
            args=[proc.stdout]
        )
    else:
        stdout = Thread(
            target=drive_stdout,
            args=[proc.stdout]
        )

    stdout.start()
    stderr.start()
    status = proc.wait()
    stdout.join()
    stderr.join()

    if status:
        print(f"{CLEAR_LINE}{RED}build failed{RESET}")
    else:
        print(f"{CLEAR_LINE}build successful")

    return status

def drive_stderr(stream):
    for line in iter(stream.readline, ""):
        print(f"[stderr] {line}", end="")

def drive_stdout_raw(stream):
    for line in iter(stream.readline, ""):
        print(f"[stdout] {line}", end="")

def drive_stdout(stream: IO[str]):
    for line in iter(stream.readline, ""):
        line_lstrip = line.lstrip()
        if line_lstrip == line:
            line = line_lstrip.rstrip()
            is_error = "error" in line or "LNK" in line
            if is_error:
                line = try_parse_compiler_error(line)
                print(f"{CLEAR_LINE}{RED}{line}{RESET}")
                continue
            is_warning = "warning" in line
            if is_warning:
                print(f"{CLEAR_LINE}{YELLOW}{line}{RESET}")
                continue
            print(f"{CLEAR_LINE}{line}")

        else:
            line = line_lstrip.rstrip()
            is_cc_progress = line.endswith(".cpp") or line.endswith(".c")
            if is_cc_progress:
                print(f"{CLEAR_LINE}  Compile: {line}\r", end="", flush=True)
                continue
            print(f"{CLEAR_LINE}[cmake] {line}")

def try_parse_compiler_error(line: str):
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
    output += RESET + "  " + line + "\n"
    if project:
        output += "  " + project + "\n"
    return output.strip()



if __name__ == "__main__":
    main()
