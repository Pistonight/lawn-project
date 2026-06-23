# copy particles, reanim and images
# usage: copypak.py TARGET_DIR EN_DIR ZH_DIR

import os
import shutil
from pathlib import Path

from .. import _common

def copy_misc():
    TARGET_DIR = _common.get_root_root() / "target" / "assets"
    pvz_dir = _common.get_packages_root() / "pvz-assets"
    EN_DIR = pvz_dir / "main12en"

    target_properties = TARGET_DIR / "shared" / "properties"
    remakedir(target_properties)
    en_properties = EN_DIR / "properties"

    (en_properties / "default.xml").copy_into(target_properties)

    # sounds are identical between EN/ZH
    target_sounds = TARGET_DIR / "shared" / "sounds"
    remakedir(target_sounds)
    en_sounds = EN_DIR / "sounds"
    copy_tree(en_sounds, target_sounds)

    # use image fonts from EN. ZH uses sys font
    target_data = TARGET_DIR / "shared" / "data"
    remakedir(target_data)
    en_data = EN_DIR / "data"
    copy_tree(en_data, target_data)

def copy_tree(from_: Path, to: Path):
    for file in os.listdir(from_):
        full_file = from_ / file
        if full_file.is_file():
            _checked_cp(full_file, to)


def copy_images():
    TARGET_DIR = _common.get_root_root() / "target" / "assets"
    pvz_dir = _common.get_packages_root() / "pvz-assets"
    EN_DIR = pvz_dir / "main12en"
    ZH_DIR = pvz_dir / "main11zh"

    PARTICLES_EXCLUDE = [
        "Sproing.png",
        "Pow.png",
        "ExplosionPowie.png",
        "ExplosionSpudow.png",
        "Doom.png",
    ]
    REANIM_EXCLUDE = set([
        "FinalWave.png",
        "SelectorScreen_Adventure_button.png",
        "SelectorScreen_Adventure_highlight.png",
        "SelectorScreen_Challenges_button.png",
        "SelectorScreen_Challenges_highlight.png",
        "SelectorScreen_StartAdventure_Button1.png",
        "SelectorScreen_StartAdventure_Highlight.png",
        "SelectorScreen_Survival_button.png",
        "SelectorScreen_Survival_highlight.png",
        "SelectorScreen_Vasebreaker_button.png",
        "SelectorScreen_vasebreaker_highlight.png",
        "SelectorScreen_WoodSign1.png",
        "SelectorScreen_WoodSign2.png",
        "SelectorScreen_WoodSign2_press.png",
        "StartPlant.png",
        "StartReady.png",
        "StartSet.png",
        "ZombiesWon.jpg",
        "ZombiesWon_.png",
    ])
    IMAGES_EXCLUDE = set([
        "Achievements_pedestal.png",
        "Achievements_pedestal_press.png",
        "Almanac.png",
        "Credits_ZombieNote.png",
        "FlagMeterLevelProgress.png",
        "PvZ_Logo.jpg",
        "PvZ_Logo_.png",
        "SelectorScreen_Almanac.png",
        "SelectorScreen_AlmanacHighlight.png",
        "SelectorScreen_Help1.png",
        "SelectorScreen_Help2.png",
        "SelectorScreen_Options1.png",
        "SelectorScreen_Options2.png",
        "SelectorScreen_Quit1.png",
        "SelectorScreen_Quit2.png",
        "SelectorScreen_Store.png",
        "SelectorScreen_StoreHighlight.png",
        "SelectorScreen_ZenGarden.png",
        "SelectorScreen_ZenGardenHighlight.png",
        "Store_NextButton.png",
        "Store_NextButtonHighlight.png",
        "Store_PrevButton.png",
        "Store_PrevButtonHighlight.png",
        "Store_Sign.png",
        "ZombieNote1.png",
        "ZombieNote2.png",
        "ZombieNote3.png",
        "ZombieNote4.png",
        "ZombieNoteHelp.png",
        "Zombiefinalnote.png",
        "acheesements_back_highlight.png",
        "acheesements_more_button.png",
        "acheesements_more_button_highlight.png",
        "acheesements_top_button.png",
        "acheesements_top_button_highlight.png",
        "options_menuback.jpg",
        "quickplay_back_button.png",
        "quickplay_back_button_highlight.png",
        "quickplay_minigames_button.png",
        "quickplay_minigames_button_highlight.png",
        "quickplay_puzzles_button.png",
        "quickplay_puzzles_button_highlight.png",
        "quickplay_survival_button.png",
        "quickplay_survival_button_highlight.png",
        "selectorscreen_achievements_bg.png",
        "zenshopbutton.png",
        "zenshopbutton_highlight.png",
    ])

    target_shared = TARGET_DIR / "shared"
    target_en = TARGET_DIR / "mainen"
    target_zh = TARGET_DIR / "mainzh"

    en = EN_DIR
    zh = ZH_DIR

    for (dir, excludes) in [
        ("particles", PARTICLES_EXCLUDE),
        ("reanim", REANIM_EXCLUDE),
        ("images", IMAGES_EXCLUDE),
    ]:
        print("==> copying " + dir)
        target_subdir = target_shared / dir
        remakedir(target_subdir)

        en_subdir = en / dir
        en_subdir_copyseparately = set(en_subdir / x for x in excludes)
        for file in os.listdir(str(en_subdir)):
            full_file = en_subdir / file
            if full_file in en_subdir_copyseparately:
                continue
            if not full_file.is_file():
                continue
            _checked_cp(full_file, target_subdir)

        target_en_subdir = target_en / dir
        remakedir(target_en_subdir)
        for file in en_subdir_copyseparately:
            _checked_cp(file, target_en_subdir)
        target_zh_subdir = target_zh / dir
        remakedir(target_zh_subdir)
        for file in excludes:
            _checked_cp(zh / dir / file, target_zh_subdir)



def remakedir(p):
    if p.exists():
        shutil.rmtree(p)
    p.mkdir(parents=True, exist_ok=True)

def _checked_cp(p: Path, t):
    try:
        p.copy_into(t)
    except:
        print("copy failed: " + str(p))
        raise
