import subprocess
import os

from src.util import _common
from src.assetbuild import _cleantxt, _copypak, _cnfont

def main(argv: list[str]) -> int:
    if len(argv) < 1:
        print(">>> usage: main.py assets <build|fix>")
        return 64
    match argv[0]:
        case "fix":
            return _run_fix()
        case "build":
            return _run_build()
        case "cnfont":
            return _run_cnfont()
    print(f">>> unknown task {argv[0]}")
    print(">>> usage: main.py assets <build|fix>")
    return 64

def _run_fix() -> int:
    assets_root = _common.get_packages_root() / "lawn-assets"
    return _cleantxt.clean(
        assets_root / "framework" / "properties" / "FrameworkStrings.txt",
        "auto",
        False
    )

def _run_cnfont() -> int:
    return _cnfont.main()

def _run_build() -> int:
    _copypak.copy_misc()
    _copypak.copy_images()

    assets_root = _common.get_packages_root() / "lawn-assets"
    target = _common.get_root_root() / "target" / "assets"
    run_dir = _common.get_root_root() / "target" / "run"
    pvz_root = _common.get_packages_root() / "pvz-assets"
    target_mainen = target / "mainen"
    target_mainzh = target / "mainzh"

    target_mainen_properties = target_mainen / "properties"
    target_mainzh_properties = target_mainzh / "properties"
    target_mainen_properties.mkdir(parents=True, exist_ok=True)
    target_mainzh_properties.mkdir(parents=True, exist_ok=True)
    status = _cleantxt.cp_cleaned(
        pvz_root / "main12en" / "properties" / "ZombatarTOS.txt",
        target_mainen_properties / "ZombatarTOS.txt",
        "ansi",
        False
    )
    if status != 0: return status
    status = _cleantxt.cp_cleaned(
        pvz_root / "main12en" / "properties" / "LawnStrings.txt",
        target_mainen_properties / "LawnStrings.txt",
        "ansi",
        False
    )
    if status != 0: return status
    status = _cleantxt.cp_cleaned(
        pvz_root / "main11zh" / "properties" / "ZombatarTOS.txt",
        target_mainzh_properties / "ZombatarTOS.txt",
        "auto",
        True
    )
    if status != 0: return status
    status = _cleantxt.cp_cleaned(
        pvz_root / "main11zh" / "properties" / "LawnStrings.txt",
        target_mainzh_properties / "LawnStrings.txt",
        "auto",
        True
    )
    if status != 0: return status

    print("==> building shaders")
    subprocess.check_call(["txtpp", "-rN", "shaders"], cwd=assets_root)
    target_shaders = target / "shared" / "shaders"
    _copypak.remakedir(target_shaders)
    for file in os.listdir(assets_root / "shaders"):
        if ".txtpp." in file:
            continue
        full_file = assets_root / "shaders" / file
        full_file.copy_into(target_shaders)

    print("==> building assets")
    mod_root = assets_root / "mod"
    for dir in os.listdir(mod_root):
        mod_dir_root = mod_root / dir
        for category in os.listdir(mod_dir_root):
            cat_root = mod_dir_root / category # this is like mod/shared/images
            target_cat_root = target / dir / category
            _copypak.copy_tree(cat_root, target_cat_root)

    _cnfont.main()

    # need to generate assets before copying framework resources
    subprocess.check_call(["pvz-bintools", "resc", "resources.yaml"], cwd=assets_root)

    framework_root = assets_root / "framework"
    for category in os.listdir(framework_root):
        cat_root = framework_root / category # this is like framework/images
        target_cat_root = target / "shared" / category
        _copypak.copy_tree(cat_root, target_cat_root)

    status = _cleantxt.cp_cleaned(
        framework_root / "properties" / "FrameworkStrings.txt",
        target / "shared" / "properties" / "FrameworkStrings.txt",
        "auto",
        False
    )
    if status != 0: return status

    subprocess.check_call(["pvz-bintools", "pakc", "--pack", str(run_dir / "shared.pak"), str(target / "shared")])
    subprocess.check_call(["pvz-bintools", "pakc", "--pack", str(run_dir / "mainen.pak"), str(target / "mainen")])
    subprocess.check_call(["pvz-bintools", "pakc", "--pack", str(run_dir / "mainzh.pak"), str(target / "mainzh")])

    return 0
