import sys
import re

keys = set()
for path in sys.argv[1:]:
    with open(path, "rb") as f:
        for line in f:
            m = re.match(rb"^\[([A-Za-z0-9_]+)\]", line.rstrip())
            if m:
                keys.add(m.group(1).decode("ascii"))

for key in sorted(keys):
    print(key)