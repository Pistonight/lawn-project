import _common
import _transformer

def copy_files():
    print("==> copying build files")
    our_root_path = get_lawn_project_root()
    our_cmake_path = our_root_path / "cmake"
    our_src_path = our_root_path / "src"

    _common.rm_rf(our_cmake_path)
    _common.rm_rf(our_src_path)
    our_cmake_path.mkdir(parents=True, exist_ok=True)
    our_src_path.mkdir(parents=True, exist_ok=True)

    upstream_root_path = _common.get_framework_root()
    (upstream_root_path / "vcpkg.json").copy_into(our_root_path)
    (upstream_root_path / "vcpkg-configuration.json").copy_into(our_root_path)
    (upstream_root_path / "cmake" / "UserConfig.cmake").copy_into(our_cmake_path)
    (upstream_root_path / "cmake" / "BuildInfo.cmake").copy_into(our_cmake_path)

    (upstream_root_path / "src" / "LawnProject.rc").copy_into(our_src_path)
    (upstream_root_path / "src" / "main_icon.ico").copy_into(our_src_path)

    _transformer.copy_transform(
        upstream_root_path / "src" / "main.cpp",
        our_src_path / "main.cpp",
    [
            _transformer.transform_includes,
            _transformer.transform_lib_includes,
            _transformer.transform_resolve_framework_includes,
        ]
    )

    print("==> formatting build files")
    _common.run_fix(our_root_path)


def get_lawn_project_root():
    return _common.get_packages_root() / "lawn"

