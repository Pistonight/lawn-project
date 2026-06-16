import shutil
import subprocess
from pathlib import Path

def get_root_root() -> Path:
    return get_packages_root().parent

def get_packages_root() -> Path:
    return Path(__file__).parent.parent.parent
