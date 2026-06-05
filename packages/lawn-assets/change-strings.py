import re
import os

script_dir = os.path.dirname(os.path.abspath(__file__))
mod_strings_path = os.path.join(script_dir, 'mod', 'properties', 'LawnStrings.txt')
main_strings_path = os.path.join(script_dir, 'main', 'properties', 'LawnStrings.txt')

def parse_strings(path):
    mapping = {}
    current_id = None
    collected = []
    with open(path, 'r', encoding="utf8", errors="surrogateescape") as f:
        for line in f:
            if re.match(r'^\[.+\]\s*$', line):
                if current_id is not None:
                    mapping[current_id] = collected
                current_id = line.strip()[1:-1]
                collected = []
            else:
                if current_id is not None:
                    collected.append(line)
    if current_id is not None:
        mapping[current_id] = collected
    return mapping

changes = parse_strings(mod_strings_path)

output = []
current_id = None
original_content = []

with open(main_strings_path, 'r', encoding="utf8", errors="surrogateescape") as f:
    for line in f:
        if re.match(r'^\[.+\]\s*$', line):
            if current_id is not None:
                output.extend(changes.get(current_id, original_content))
            current_id = line.strip()[1:-1]
            original_content = []
            output.append(line)
        else:
            original_content.append(line)

if current_id is not None:
    output.extend(changes.get(current_id, original_content))

with open(main_strings_path, 'w', encoding="utf8", errors="surrogateescape") as f:
    f.writelines(output)
