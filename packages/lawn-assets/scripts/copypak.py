# copy particles, reanim and images
# usage: copypak.py TARGET_DIR EN_DIR ZH_DIR

import sys
import shutil
from pathlib import Path

def main():
    if len(sys.argv) < 2:
        print("BAD USAGE!")
        exit(64)
    TARGET_DIR, EN_DIR, ZH_DIR = sys.argv[1:]

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

    target = Path(TARGET_DIR)
    target_shared = target / "shared"
    target_en = target / "mainen"
    target_zh = target / "mainzh"

    en = Path(EN_DIR)
    zh = Path(ZH_DIR)

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
        for file in en_subdir.iterdir():
            if file in en_subdir_copyseparately:
                continue
            checked_cp(file, target_subdir)

        target_en_subdir = target_en / dir
        remakedir(target_en_subdir)
        for file in en_subdir_copyseparately:
            checked_cp(file, target_en_subdir)
        target_zh_subdir = target_zh / dir
        remakedir(target_zh_subdir)
        for file in excludes:
            checked_cp(zh / dir / file, target_zh_subdir)



def remakedir(p):
    if p.exists():
        shutil.rmtree(p)
    p.mkdir(parents=True, exist_ok=True)

def checked_cp(p: Path, t):
    try:
        p.copy_into(t)
    except:
        print("copy failed: " + str(p))
        raise



if __name__ == "__main__":
    main()
