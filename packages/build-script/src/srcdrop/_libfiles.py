import os
from pathlib import Path

from .. import _common, _fmt
from . import _transformer

def copy_files():
    print(f"{_fmt.GREEN}==> copying lib files{_fmt.RESET}")
    root_path = _get_target_project_root()
    target_src_path = root_path / "thirdparty" / "src"
    _common.rm_rf(target_src_path)
    _common.rm_rf(root_path / "src")

    framework_path = _common.get_upstream_root()
    thirdparty_src_path = framework_path / "thirdparty" / "src"

    exclude = set([
        # we are tracking our own bass lib
        thirdparty_src_path / "bass.h"
    ])
    for (dirpath, _, filenames) in os.walk(thirdparty_src_path):
        # future: brittle: ffmpeg: we use vcpkg
        currdir_name = os.path.basename(dirpath)
        if currdir_name.startswith("libav") or currdir_name.startswith("libsw"):
            continue

        dirpath = Path(dirpath)
        reldirpath = dirpath.relative_to(thirdparty_src_path)

        target_src_dirpath = target_src_path / reldirpath
        target_src_dirpath.mkdir(parents = True, exist_ok=True)

        for file in filenames:
            filepath = dirpath / file
            if filepath in exclude:
                continue
            filepath.copy_into(target_src_dirpath)

    transformers = _get_transformers()

    imagelib_src_path = framework_path / "src" / "ImageLib"
    imagelib_target_path = root_path / "src" / "ImageLib"
    for (dirpath, _, filenames) in os.walk(imagelib_src_path):
        dirpath = Path(dirpath)
        reldirpath = dirpath.relative_to(imagelib_src_path)
        target_src_dirpath = imagelib_target_path / reldirpath
        target_src_dirpath.mkdir(parents = True, exist_ok=True)
        for file in filenames:
            if file.endswith("CMakeLists.txt"):
                continue
            filepath = dirpath / file
            _transformer.copy_transform(filepath, target_src_dirpath / file, transformers)


    paklib_src_path = framework_path / "src" / "PakLib"
    paklib_target_path = root_path / "src" / "PakLib"
    for (dirpath, _, filenames) in os.walk(paklib_src_path):
        dirpath = Path(dirpath)
        reldirpath = dirpath.relative_to(paklib_src_path)
        target_src_dirpath = paklib_target_path / reldirpath
        target_src_dirpath.mkdir(parents = True, exist_ok=True)
        for file in filenames:
            if file.endswith("CMakeLists.txt"):
                continue
            filepath = dirpath / file
            _transformer.copy_transform(filepath, target_src_dirpath / file, transformers)

    run_fix()


def transform():
    print(f"{_fmt.GREEN}==> transforming lib files{_fmt.RESET}")
    our_root_path = _get_target_project_root()
    transformers = _get_transformers()
    for (dirpath, _, filenames) in os.walk(our_root_path / "src"):
        for file in filenames:
            if _common.is_src_file(file) or _common.is_header_file(file):
                _transformer.transform(Path(dirpath) / file, transformers)

    run_fix()


def run_fix():
    print(f"{_fmt.CYAN}==> formatting lib files{_fmt.RESET}")
    _transformer.run_fix(_get_target_project_root())


def _get_transformers():
    return [
        _transformer.transform_includes,
        _transformer.transform_lib_includes,
    ]


def _get_target_project_root():
    return _common.get_packages_root() / "resod-lib"

