# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Pistonight/lawn-project contributors

import sys
from pathlib import Path
import subprocess

def get_in_source():
    texts = set()
    plants = [
        "PEASHOOTER",
        "SUNFLOWER",
        "CHERRY_BOMB",
        "WALL_NUT",
        "POTATO_MINE",
        "SNOW_PEA",
        "CHOMPER",
        "REPEATER",
        "PUFF_SHROOM",
        "SUN_SHROOM",
        "FUME_SHROOM",
        "GRAVE_BUSTER",
        "HYPNO_SHROOM",
        "SCAREDY_SHROOM",
        "ICE_SHROOM",
        "DOOM_SHROOM",
        "LILY_PAD",
        "SQUASH",
        "THREEPEATER",
        "TANGLE_KELP",
        "JALAPENO",
        "SPIKEWEED",
        "TORCHWOOD",
        "TALL_NUT",
        "SEA_SHROOM",
        "PLANTERN",
        "CACTUS",
        "BLOVER",
        "SPLIT_PEA",
        "STARFRUIT",
        "PUMPKIN",
        "MAGNET_SHROOM",
        "CABBAGE_PULT",
        "FLOWER_POT",
        "KERNEL_PULT",
        "COFFEE_BEAN",
        "GARLIC",
        "UMBRELLA_LEAF",
        "MARIGOLD",
        "MELON_PULT",
        "GATLING_PEA",
        "TWIN_SUNFLOWER",
        "GLOOM_SHROOM",
        "CATTAIL",
        "WINTER_MELON",
        "GOLD_MAGNET",
        "SPIKEROCK",
        "COB_CANNON",
        "IMITATER",
        "EXPLODE_O_NUT",
        "GIANT_WALLNUT",
        "SPROUT",
        "REPEATER"
    ]
    for p in plants:
        texts.add("[" + p + "]")
        texts.add("[" + p + "_DESCRIPTION]")
        texts.add("[" + p + "_TOOLTIP]")
    output = subprocess.check_output(["rg", "\"\\[[A-Z_]+\\]\"", "../lawn-app/src", "-IoN"], text=True)
    for line in output.split('\n'):
        key = line.strip()
        if key.startswith("\""):
            key = key[1:]
        if key.endswith("\""):
            key = key[:-1]
        if key:
            texts.add(key)
    return texts

def get_in_res():
    texts = set()
    output = subprocess.check_output(["rg", "\\[[A-Z_]+\\]", "main", "-IoN"], text=True)
    for line in output.split('\n'):
        key = line.strip()
        if key:
            texts.add(key)
    output = subprocess.check_output(["rg", "\\[[A-Z_]+\\]", "mod", "-IoN"], text=True)
    for line in output.split('\n'):
        key = line.strip()
        if key:
            texts.add(key)
    return texts

in_res = get_in_res()
in_source = get_in_source()
missing = []
ok = True

for key in in_source:
    if key not in in_res:
        missing.append(key)
        ok = False

for x in sorted(missing):
    print(x)

if not ok:
    exit(1)

