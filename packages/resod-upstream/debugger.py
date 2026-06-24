from pathlib import Path
import sys
sys.path.insert(0, str((Path(__file__).parent.parent / "build-script").resolve()))

from resod_upstream_debugger import main # type: ignore[reportMissingImports]
main()
