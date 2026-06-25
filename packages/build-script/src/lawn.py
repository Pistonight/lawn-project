import shutil
from datetime import datetime
from pathlib import Path

from util import _debugger, _common, _build, _fmt

# ResoddedFramework has x86 presets but we only build x64
PRESET = "windows-msvc-x64"
def main(argv: list[str]) -> int:
    if len(argv) < 1:
        print(">>> usage: main.py lawn <build|devenv> ARGS")
        return 64
    match argv[0]:
        case "build":
            return _run_build(argv[1:])
        case "devenv":
            return _run_devenv(argv[1:])
    print(f">>> unknown task {argv[0]}")
    print(">>> usage: main.py lawn <build|devenv> ARGS")
    return 64

def _run_devenv(argv: list[str]) -> int:
    is_release="release" in argv or "--release" in argv

    repo_root = _common.get_root_root()
    flavor = "release" if is_release else "debug"
    cmake_dir = repo_root / "target" / "lawn" / flavor
    exe = cmake_dir / "out" / "PlantsVsZombies.exe"
    return _debugger.launch_debugger_project(cmake_dir, exe)


def _run_build(argv: list[str]) -> int:
    args = _build.parse_args(argv)
    if args.is_help:
        _build.print_arg_help()
        return 64
    if args.is_x86:
        print(">>> x86 is not supported")
        return 3

    flavor = "release" if args.is_release else "debug"

    repo_root = _common.get_root_root()
    ninja_dir = repo_root / "build"
    lawn_root = _common.get_packages_root() / "lawn"
    build_root = repo_root / "target" / "lawn"
    build_dir = build_root / flavor
    run_dir = repo_root / "target" / "run"

    target_flavors = ["debug", "release"]  if args.is_all else [ flavor ]

    if args.is_clean:
        if ninja_dir.exists():
            print("==> cleaning ninja configure dir")
            shutil.rmtree(ninja_dir)
        for flavor in target_flavors:
            build_dir = build_root / flavor
            if build_dir.exists():
                print(f"==> cleaning build dir ({flavor})")
                shutil.rmtree(build_dir)
        if args.is_all:
            print("==> all cleaned!")
            return 0

    # source generation should happen before configuring
    _generate_buildinfo_inc()

    if args.is_clean or args.do_ninja or not _build.is_dir_configured(ninja_dir):
        status = _build.cmake_configure_ninja(
            lawn_root,
            PRESET,
            ninja_dir,
            args.is_release,
            args.is_raw
        )
        if status != 0:
            return status

    for flavor in target_flavors:
        build_dir = build_root / flavor
        if args.is_clean or args.is_configure or not _build.is_dir_configured(build_dir):
            status = _build.cmake_configure_msvc(
                lawn_root,
                PRESET,
                build_dir,
                False,
                flavor == "release",
                args.is_raw
            )
            if status != 0:
                return status

    for flavor in target_flavors:
        print(f"{_fmt.PINK}==> building lawn ({flavor}){_fmt.RESET}")
        build_dir = build_root / flavor
        status = _build.cmake_build(build_dir, flavor == "release", args.is_raw)
        if status != 0:
            return status

    # if release then we copy the release exe to run dir, otherwise copy debug
    run_dir.mkdir(parents=True, exist_ok=True)
    flavor = "release" if args.is_release else "debug"
    exe = build_root / flavor / "out" / "PlantsVsZombies.exe"
    if not exe.exists():
        print(f"{_fmt.RED}>>> error: cannot find EXE!{_fmt.RESET}")
        print(f"{_fmt.RED}>>> {str(exe)}{_fmt.RESET}")
        return 1
    exe.copy_into(run_dir)

    if args.is_all:
        print("==> all build done!")


    return 0

def _generate_buildinfo_inc():
    version_file = _common.get_root_root() / "LawnVersion.txt"
    output_inc_file = _common.get_packages_root() / "resod-framework" / "src-mixin" / "SexyAppFramework" / "BuildInfo.inc"
    version = version_file.read_text().strip()
    print("==> version: " + version)
    output_inc_file = Path(output_inc_file)
    # not making parent because it should exist
    repo_root = _common.get_root_root();
    git_commit = _common.git_head_hash(repo_root)
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

    output_inc_file.write_text(content, encoding="utf8", newline="\n")
