from pathlib import Path
import sys
sys.path.insert(0, str((Path(__file__).parent.parent / "build-script").resolve()))

from update_resod_upstream import run_transform # type: ignore[reportMissingImports]
run_transform()
