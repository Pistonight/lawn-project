from src import _common
from src.assetbuild import _cleantxt

def main():
    assets_root = _common.get_packages_root() / "lawn-assets"

    _cleantxt.clean(
        assets_root / "framework" / "properties" / "FrameworkStrings.txt",
        "auto",
        False
    )
