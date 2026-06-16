import os
from pathlib import Path

from .. import _common
from . import _transformer

def copy_files():
    print("==> copying build files")
    our_root_path = _get_target_project_root()
    our_cmake_path = our_root_path / "cmake"
    our_src_path = our_root_path / "src"

    _common.rm_rf(our_cmake_path)
    _common.rm_rf(our_src_path)
    our_cmake_path.mkdir(parents=True, exist_ok=True)
    our_src_path.mkdir(parents=True, exist_ok=True)

    upstream_root_path = _common.get_upstream_root()
    (upstream_root_path / "vcpkg.json").copy_into(our_root_path)
    (upstream_root_path / "vcpkg-configuration.json").copy_into(our_root_path)
    (upstream_root_path / "cmake" / "UserConfig.cmake").copy_into(our_cmake_path)

    (upstream_root_path / "src" / "main_icon.ico").copy_into(our_src_path)
    (upstream_root_path / "src" / "SexyAppFramework" / "resource.h").copy_into(our_src_path)

    transformers = _get_transformers()

    _transformer.copy_transform(
        upstream_root_path / "src" / "main.cpp",
        our_src_path / "main.cpp",
        transformers
    )
    _transformer.copy_transform(
        upstream_root_path / "src" / "LawnProject.rc",
        our_src_path / "LawnProject.rc",
        transformers
    )

    run_fix()


def transform():
    print("==> transforming build files")
    our_root_path = _get_target_project_root()
    transformers = _get_transformers()
    for (dirpath, _, filenames) in os.walk(our_root_path / "src"):
        for file in filenames:
            if _common.is_src_file(file) or _common.is_header_file(file):
                _transformer.transform(Path(dirpath) / file, transformers)

    run_fix()

def run_fix():
    print("==> formatting build files")
    _transformer.run_fix(_get_target_project_root())

def _get_transformers():
    return [
        _transformer.transform_includes,
        _transformer.transform_lib_includes,
        _transformer.transform_resolve_framework_includes,
    ]

def _get_target_project_root():
    return _common.get_packages_root() / "lawn"

