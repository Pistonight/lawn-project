import sys

from src import _debugger, _common

def main():
    is_release="release" in sys.argv or "--release" in sys.argv
    is_x86="x86" in sys.argv

    project_root = _common.get_packages_root() / "resod-upstream"
    arch = "x86" if is_x86 else "x64"
    flavor = "release" if is_release else "debug"
    cmake_dir = project_root / f"{arch}-{flavor}"
    exe = project_root / "ResoddedFramework" / "Export" / "LawnProject.exe"
    _debugger.launch_debugger_project(cmake_dir, exe)


