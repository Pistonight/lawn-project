import os
from pathlib import Path

from ..util import _common, _fmt
from . import _transformer

def copy_files():
    print(f"{_fmt.GREEN}==> copying asset files{_fmt.RESET}")
    our_root_path = _get_target_project_root()
    our_assets_path = our_root_path / "framework"

    _common.rm_rf(our_assets_path)

    upstream_root_path = _common.get_upstream_root()
    upstream_assets_path = upstream_root_path / "assets"

    for asset_dir in ["images", "properties"]:
        upstream_assets_sub_path = upstream_assets_path / asset_dir
        for (dirpath, _, filenames) in os.walk(upstream_assets_sub_path):
            dirpath = Path(dirpath)
            reldirpath = dirpath.relative_to(upstream_assets_path)

            target_dirpath = our_assets_path / reldirpath
            target_dirpath.mkdir(parents = True, exist_ok=True)

            for file in filenames:
                filepath = dirpath / file
                filepath.copy_into(target_dirpath)

    run_fix()

def transform():
    run_fix()

def run_fix():
    print(f"{_fmt.CYAN}==> formatting asset files{_fmt.RESET}")
    _transformer.run_fix(_get_target_project_root())

def _get_target_project_root():
    return _common.get_packages_root() / "lawn-assets"

