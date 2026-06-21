import sys

USAGE = """\
usage: cleantxt.py <encoding> <file> [output] [--lang]
  encoding can be:
    ansi - Windows ANSI (cp1252)
    auto - Automatic: UTF16 with BOM, UTF8 with BOM, UTF8
  output defaults to <file> (in-place)
  options:
    --lang   Replace certain characters for better support with our fonts"""



def main():
    args = sys.argv[1:]
    is_lang = "--lang" in args
    positional = [a for a in args if not a.startswith("--")]
    if len(positional) not in (2, 3):
        print(USAGE, file=sys.stderr)
        sys.exit(1)
    encoding, file = positional[0], positional[1]
    output = positional[2] if len(positional) == 3 else file

    with open(file, "rb") as f:
        data = f.read()

    try:
        text = decode(data, encoding)
    except ValueError as e:
        print(e, file=sys.stderr)
        print(USAGE, file=sys.stderr)
        sys.exit(1)

    if is_lang:
        text = clean_lang(text)
    else:
        text = "\n".join(text.splitlines())

    with open(output, "wb") as f:
        f.write(text.encode("utf-8"))

def decode(data: bytes, encoding: str) -> str:
    if encoding == "ansi":
        return data.decode("cp1252")
    if encoding == "auto":
        if data[:2] in (b"\xff\xfe", b"\xfe\xff"):
            return data.decode("utf-16")
        if data[:3] == b"\xef\xbb\xbf":
            return data.decode("utf-8-sig")
        return data.decode("utf-8")
    raise ValueError(f"unknown encoding: {encoding}")

def clean_lang(input: str):
    lines = []
    for l in input.splitlines():
        l = l.strip()
        # replace chinese punctuations since some fonts don't support them
        l = l.replace("？", "? ").replace("！", "! ").replace("。", ". ").replace("，", ", ")
        lines.append(l.strip())
    return "\n".join(lines)


if __name__ == "__main__":
    main()
