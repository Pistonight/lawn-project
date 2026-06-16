import sys
import shutil

from src import _version, _common, _build

def main():
    args = _build.parse_args(sys.argv)
    if args.is_help:
        _build.print_arg_help()
        exit(64)
    if args.is_x86:
        print(">>> x86 is not supported")
        exit(3)

    flavor = "release" if args.is_release else "debug"

    repo_root = _common.get_root_root()
    ninja_dir = repo_root / "build"
    lawn_root = _common.get_packages_root() / "lawn"
    build_root = repo_root / "target" / "lawn"
    build_dir = build_root / flavor

    if args.is_clean:
        if ninja_dir.exists():
            print("==> cleaning ninja configure dir")
            shutil.rmtree(ninja_dir)
        if args.is_all:
            for flavor in ["debug", "release"]:
                for arch in ["x64", "x86"]:
                    build_dir = build_root / f"{flavor}-{arch}"
                    if build_dir.exists():
                        shutil.rmtree(build_dir)
            print("==> all cleaned!")
            return 
        if build_dir.exists():
            print("==> cleaning target build dir (--all to clean all targets)")
            shutil.rmtree(build_dir)

    PRESET = "windows-msvc-x64"
    if args.is_clean or args.do_ninja or not _build.is_dir_configured(ninja_dir):
        status = _build.cmake_configure_ninja(
            lawn_root,
            PRESET,
            ninja_dir,
            args.is_release,
            args.is_raw
        )
        if status != 0:
            exit(status)

    if args.is_clean or args.is_configure or not _build.is_dir_configured(build_dir):
        status = _build.cmake_configure_msvc(
            lawn_root,
            PRESET,
            build_dir,
            args.is_x86,
            args.is_raw
        )
        if status != 0:
            exit(status)

    _gen_buildinfo_inc()

    print(f"==> building lawn ({"release" if args.is_release else "debug"})")
    status = _build.cmake_build(build_dir, args.is_release, args.is_raw)
    exit(status)

def _gen_buildinfo_inc():
    # we are keeping it here for reference to ResoddedFramework since
    # it generates it there (at root of repo)
    version_file = _common.get_root_root() / "LawnVersion.txt"
    inc_file = _common.get_packages_root() / "resod-framework" / "src-mixin" / "SexyAppFramework" / "BuildInfo.inc"
    _version.generate_buildinfo_inc(version_file, inc_file)
