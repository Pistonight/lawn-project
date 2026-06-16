import sys
import shutil

from src import _common, _build

PINK = "\033[95m"
RESET = "\033[0m"

def main():
    args = _build.parse_args(sys.argv)
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
            return 

    if args.is_clean or args.do_ninja or not _build.is_dir_configured(ninja_dir):
        status = _build.cmake_configure_ninja(
            upstream_root,
            f"windows-{arch}-{flavor}-msvc",
            ninja_dir,
            args.is_release,
            args.is_raw
        )
        if status != 0:
            exit(status)

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
                exit(status)

    for dir in target_dirs:
        # for consistency set the same build number cache
        (upstream_root / "cmake" / "BuildNumberCache.txt").write_text("41")

        build_dir = upstreampkg_root / dir
        print(f"{PINK}==> building resodded ({dir}){RESET}")
        status = _build.cmake_build(build_dir, "release" in dir, args.is_raw)
        if status != 0:
            exit(status)

