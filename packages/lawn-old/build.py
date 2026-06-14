import os
import sys
import subprocess
from threading import Thread
from typing import IO

CLEAR_LINE = "\r\033[2K"
RED = "\033[31m"
YELLOW = "\033[33m"
RESET = "\033[0m"

def nproc():
    return os.cpu_count()

def drive_cmake(build_dir, config_name, is_raw):
    command = [
        "cmake", "--build", build_dir,
        "--config", config_name,
        "--",
        f"/m:{nproc()}"
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
    if len(sys.argv) < 3:
        print("error: usage: build.py BUILD_DIR CONFIG")
        exit(64)
    raw = len(sys.argv) > 3 and sys.argv[3] == "--raw"
    exit(drive_cmake(sys.argv[1], sys.argv[2], raw))
