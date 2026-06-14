from pathlib import Path
from typing import Callable

import _upstream

def copy_transform(
        from_path: Path, 
        to_path: Path,
        transformers: list[ Callable[ [Path, list[str]], list[str] ] ]):
    file = from_path.read_text(encoding = "utf8")
    lines = [x.rstrip() for x in file.splitlines()]
    for t in transformers:
        lines = t(from_path, lines)
    good = "\n".join(lines)
    to_path.write_text(good, encoding ="utf8", newline="\n")



def transform_lib_includes(_: Path, lines: list[str]) -> list[str]:
    for i in range(len(lines)):
        inc_part = _parse_include_line(lines[i])
        if not inc_part:
            continue
        good_inc_part = None
        if inc_part[:-1].endswith("ImageLib.h"):
            good_inc_part = "<ImageLib/ImageLib.h>"
        elif inc_part[:-1].endswith("PakInterface.h"):
            good_inc_part = "<PakLib/PakInterface.h>"

        if not good_inc_part:
            continue
        lines[i] = "#include " + good_inc_part
    return lines

def transform_resolve_framework_includes(from_path: Path, lines: list[str]) -> list[str]:
    for i in range(len(lines)):
        inc_part = _parse_include_line(lines[i])
        if not inc_part:
            continue
        resolved_inc = _resolve_include(from_path, inc_part[1:-1])
        if not resolved_inc:
            continue
        if inc_part == resolved_inc:
            continue
        lines[i] = "#include " + resolved_inc
    return lines



def transform_includes(_: Path, lines: list[str]) -> list[str]:
    for i in range(len(lines)):
        inc_part = _parse_include_line(lines[i])
        if not inc_part:
            continue

        good_inc_part = inc_part.replace("\\", "/")

        if inc_part == good_inc_part:
            continue
        lines[i] = "#include " + good_inc_part
    return lines

def _parse_include_line(line: str) -> str | None:
    line = line.strip()
    if not line.startswith("#include"):
        return None
    line = line[len("#include"):].strip()
    if not line.endswith(">") and not line.endswith("\""):
        return None
    return line

def _resolve_include(from_path: Path, include: str) -> str | None:
    # resolve relatively?
    resolve_base = from_path.parent
    try:
        resolved = (resolve_base / include).resolve()
    except:
        # maybe doesn't exist, keep existing
        return None
    if not resolved.exists():
        return None
    try:
        # find the base path
        project_base = resolved.parent
        while project_base.name != "src":
            project_base = project_base.parent
        resolved_rel_to_base = resolved.relative_to(project_base)
        # if it's in the src (root), use "LawnApp"
        if resolved.parent == project_base:
            return f"<LawnApp/{resolved.name}>"

        resolved_rel_to_base = str(resolved_rel_to_base)
        for k in _upstream.UPSTREAM_LIBS:
            if resolved_rel_to_base.startswith(k):
                return f"<{resolved_rel_to_base.replace('\\', '/')}>"
        raise Exception("resolved to a path that is not a known lib: " + resolved_rel_to_base)

    except:
        print("unable to resolve " + include + " from " + str(from_path))
        raise


