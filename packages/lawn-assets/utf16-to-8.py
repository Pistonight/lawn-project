import sys

with open(sys.argv[1], "rb") as f:
    data = f.read()

text = data.decode("utf-16")
# clean it
lines = []
for l in text.splitlines():
    l = l.strip()
    # replace chinese punctuations since some fonts don't support them
    l = l.replace("？", "? ").replace("！", "! ").replace("。", ". ").replace("，", ", ")
    lines.append(l.strip())
with open(sys.argv[2], "w", encoding="utf-8") as f:
    f.write("\n".join(lines))
