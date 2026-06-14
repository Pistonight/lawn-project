import os
from pathlib import Path

import _common
import _transformer

def copy_files():
    print("==> copying lib files")
    root_path = get_lib_project_root()
    target_src_path = root_path / "thirdparty" / "src"
    target_include_path = root_path / "thirdparty" / "include"
    _common.rm_rf(target_src_path)
    _common.rm_rf(target_include_path)
    _common.rm_rf(root_path / "src")

    framework_path = _common.get_framework_root()
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

        target_include_dirpath = target_include_path / reldirpath
        target_src_dirpath = target_src_path / reldirpath

        target_include_dirpath.mkdir(parents = True, exist_ok=True)
        if any(_common.is_src_file(x) for x in filenames):
            # ^ doesn't consider exclude but good enough
            target_src_dirpath.mkdir(parents = True, exist_ok=True)

        for file in filenames:
            filepath = dirpath / file
            if filepath in exclude:
                continue
            if _common.is_src_file(file):
                filepath.copy_into(target_src_dirpath)
            else:
                filepath.copy_into(target_include_dirpath)

    lib_transformers = [
        _transformer.transform_includes,
        _transformer.transform_lib_includes,
    ]

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
            _transformer.copy_transform(filepath, target_src_dirpath / file, lib_transformers)


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
            _transformer.copy_transform(filepath, target_src_dirpath / file, lib_transformers)

    print("==> formatting lib files")
    _common.run_fix(root_path)

def get_lib_project_root():
    return _common.get_packages_root() / "resod-lib"

