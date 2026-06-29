import shutil
from src.util import _debugger, _common, _build, _fmt

def main(argv: list[str]) -> int:
    if len(argv) < 1:
        print(">>> usage: main.py upstream <build|devenv|upgrade|transform> ARGS")
        return 64
    match argv[0]:
        case "upgrade":
            import srcdrop.main as _srcdrop
            return _srcdrop.main(argv[1:])
        case "devenv":
            return _run_devenv(argv[1:])
        case "build":
            return _run_build(argv[1:])
        case "transform":
            return _run_transform()
    print(f">>> unknown task {argv[0]}")
    print(">>> usage: main.py upstream <build|devenv|upgrade|transform> ARGS")
    return 64

def _run_devenv(argv: list[str]) -> int:
    is_release="release" in argv or "--release" in argv
    is_x86="x86" in argv
    project_root = _common.get_packages_root() / "resod-upstream"
    arch = "x86" if is_x86 else "x64"
    flavor = "release" if is_release else "debug"
    cmake_dir = project_root / f"{arch}-{flavor}"
    exe = project_root / "ResoddedFramework" / "Export" / "LawnProject.exe"
    return _debugger.launch_debugger_project(cmake_dir, exe)

def _run_transform() -> int:
    import srcdrop.main as _srcdrop
    _srcdrop.run_transform()
    return 0

def _run_build(argv: list[str]) -> int:
    args = _build.parse_args(argv)
    if args.is_help:
        _build.print_arg_help()
        exit(64)

    flavor = "release" if args.is_release else "debug"
    arch = "x86" if args.is_x86 else "x64"

    upstreampkg_root = _common.get_upstreampkg_root()
    upstream_root = _common.get_upstream_root()
    ninja_dir = upstreampkg_root / "build"

    target_archs = ["x86", "x64"] if args.is_all else [ arch ]
    target_flavors = ["debug", "release"] if args.is_all else [ flavor ]

    target_dirs = []
    for flavor in target_flavors:
        for arch in target_archs:
            target_dirs.append(f"{arch}-{flavor}")

    if args.is_clean:
        if ninja_dir.exists():
            print("==> cleaning ninja configure dir")
            shutil.rmtree(ninja_dir)
        for dir in target_dirs:
            build_dir = upstreampkg_root / dir
            if build_dir.exists():
                print(f"==> cleaning build dir ({dir})")
                shutil.rmtree(build_dir)
        if args.is_all:
            print("==> all cleaned!")
            return 0

    ninja_dirty = False
    if not args.is_release:
        for dir in target_dirs:
            build_dir = upstreampkg_root / dir
            if _build.is_ninja_dirty_in_build_dir(build_dir):
                ninja_dirty = True
                break

    if ninja_dirty or args.is_clean or args.do_ninja or not _build.is_dir_configured(ninja_dir):
        status = _build.cmake_configure_ninja(
            upstream_root,
            f"windows-{arch}-{flavor}-msvc",
            ninja_dir,
            args.is_release,
            args.is_raw
        )
        if status != 0:
            return status

    for dir in target_dirs:
        build_dir = upstreampkg_root / dir
        if args.is_clean or args.is_configure or not _build.is_dir_configured(build_dir):
            status = _build.cmake_configure_msvc(
                upstream_root,
                f"windows-{dir}-msvc",
                build_dir,
                "x86" in dir,
                "release" in dir,
                args.is_raw
            )
            if status != 0:
                return status

    for dir in target_dirs:
        # for consistency set the same build number cache
        (upstream_root / "cmake" / "BuildNumberCache.txt").write_text("41")

        build_dir = upstreampkg_root / dir
        print(f"{_fmt.PINK}==> building resodded ({dir}){_fmt.RESET}")
        status = _build.cmake_build(build_dir, "release" in dir, args.is_raw)
        if status != 0:
            return status

    return 0

