from src.util import _fmt


def clean(file, encoding: str, is_lang: bool) -> int:
    return cp_cleaned(file, file, encoding, is_lang)


def cp_cleaned(file_in, file_out, encoding: str, is_lang: bool) -> int:
    with open(file_in, "rb") as f:
        data = f.read()

    try:
        text = _decode(data, encoding)
    except ValueError:
        print(f"{_fmt.RED}>>> error: cannot decode {file_in} using encoding '{encoding}'{_fmt.RESET}")
        return 1

    if is_lang:
        text = _clean_lang(text)
    else:
        text = "\n".join(text.splitlines())

    with open(file_out, "wb") as f:
        f.write(text.encode("utf-8"))

    return 0

def _decode(data: bytes, encoding: str) -> str:
    if encoding == "ansi":
        return data.decode("cp1252")
    if encoding == "auto":
        if data[:2] in (b"\xff\xfe", b"\xfe\xff"):
            return data.decode("utf-16")
        if data[:3] == b"\xef\xbb\xbf":
            return data.decode("utf-8-sig")
        return data.decode("utf-8")
    raise ValueError(f"unknown encoding: {encoding}")

def _clean_lang(input: str):
    lines = []
    for l in input.splitlines():
        l = l.strip()
        # replace chinese punctuations since some fonts don't support them (no longer needed)
        # l = l.replace("？", "? ").replace("！", "! ").replace("。", ". ").replace("，", ", ")
        lines.append(l.strip())
    return "\n".join(lines)
