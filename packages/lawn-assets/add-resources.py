
import re
import os

script_dir = os.path.dirname(os.path.abspath(__file__))
add_resources_path = os.path.join(script_dir, 'mod', 'properties', 'resources.xml')
resources_path = os.path.join(script_dir, 'main', 'properties', 'resources.xml')

# Parse add-resources.xml: collect lines between each <Resources>...</Resources> pair, keyed by id
additions = {}
current_id = None
collected = []

with open(add_resources_path, 'r') as f:
    for line in f:
        has_open = '<Resources' in line
        has_close = '</Resources>' in line
        if has_open and not has_close:
            m = re.search(r'<Resources\s+id="([^"]+)"', line)
            if m:
                current_id = m.group(1)
                collected = []
        elif has_close and not has_open:
            if current_id is not None:
                additions[current_id] = collected
            current_id = None
            collected = []
        elif current_id is not None:
            collected.append(line)

# Process resources.xml: insert additions before each </Resources> closing tag
output = []
current_id = None

with open(resources_path, 'r') as f:
    for line in f:
        has_open = '<Resources' in line
        has_close = '</Resources>' in line
        if has_open and not has_close:
            m = re.search(r'<Resources\s+id="([^"]+)"', line)
            if m:
                current_id = m.group(1)
            output.append(line)
        elif has_close and not has_open:
            if current_id in additions:
                output.extend(additions[current_id])
            current_id = None
            output.append(line)
        else:
            output.append(line)

with open(resources_path, 'w') as f:
    f.writelines(output)
