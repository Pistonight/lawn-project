import subprocess
import collections
import shutil
import sys
from pathlib import Path

CLEAR_LINE = "\r\033[2K"
RED = "\033[31m"
GREEN = "\033[32m"
YELLOW = "\033[33m"
CYAN = "\033[36m"
PINK = "\033[95m"
RESET = "\033[0m"

def subprocess_call_rolling_window(command: list[str|Path], cwd: str | Path | None) -> int:
    # make an 8-line window that prints the last 8 lines of the output
    # (scrolling as the output goes) in yellow. when it's done it's cleared
    proc = subprocess.Popen(
        command,
        cwd=cwd,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT,
        text=True,
        bufsize=1
    )

    lines: collections.deque[str] = collections.deque(maxlen=8)
    drawn = 0

    def redraw():
        nonlocal drawn
        width = shutil.get_terminal_size().columns
        # move the cursor back up to the top of the window we last drew
        if drawn:
            sys.stdout.write(f"\033[{drawn}A")
        for line in lines:
            # truncate so long lines don't wrap and desync the cursor math
            sys.stdout.write(f"{CLEAR_LINE}{YELLOW}{line[:width]}{RESET}\n")
        drawn = len(lines)
        sys.stdout.flush()

    stdout: IO[str] = proc.stdout # type: ignore

    for raw in iter(stdout.readline, ""):
        lines.append(raw.rstrip())
        redraw()

    status = proc.wait()

    # clear the window: go back to the top, wipe each line, return to the top
    if drawn:
        sys.stdout.write(f"\033[{drawn}A")
        for _ in range(drawn):
            sys.stdout.write(f"{CLEAR_LINE}\n")
        sys.stdout.write(f"\033[{drawn}A")
        sys.stdout.flush()

    return status

