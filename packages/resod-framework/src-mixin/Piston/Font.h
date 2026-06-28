#pragma once

#include <LawnApp/Resources.h>

#include <Piston/System.h>

namespace Sexy {
class Font;
} // namespace Sexy

namespace Piston {

namespace FontLoader {

// initialize fonts after they are loaded
void InitLoaderBarFonts();
void InitLoadingFonts();

} // namespace FontLoader

inline Sexy::Font* MapZhFont(Sexy::Font* input) {
    if (!System::IsChinese()) {
        return input;
    }
    if (input == Sexy::FONT_BRIANNETOD12) {
        return Sexy::FONT_BRIANNETOD12ZH;
    }
    if (input == Sexy::FONT_BRIANNETOD32) {
        return Sexy::FONT_BRIANNETOD32ZH;
    }
    if (input == Sexy::FONT_BRIANNETOD32BLACK) {
        return Sexy::FONT_BRIANNETOD32BLACKZH;
    }
    if (input == Sexy::FONT_CONTINUUMBOLD14) {
        return Sexy::FONT_CONTINUUMBOLD14ZH;
    }
    if (input == Sexy::FONT_CONTINUUMBOLD14OUTLINE) {
        return Sexy::FONT_CONTINUUMBOLD14OUTLINEZH;
    }
    if (input == Sexy::FONT_DWARVENTODCRAFT12) {
        return Sexy::FONT_DWARVENTODCRAFT12ZH;
    }
    if (input == Sexy::FONT_DWARVENTODCRAFT15) {
        return Sexy::FONT_DWARVENTODCRAFT15ZH;
    }
    if (input == Sexy::FONT_DWARVENTODCRAFT18) {
        return Sexy::FONT_DWARVENTODCRAFT18ZH;
    }
    if (input == Sexy::FONT_DWARVENTODCRAFT18BRIGHTGREENINSET) {
        return Sexy::FONT_DWARVENTODCRAFT18BRIGHTGREENINSETZH;
    }
    if (input == Sexy::FONT_DWARVENTODCRAFT18GREENINSET) {
        return Sexy::FONT_DWARVENTODCRAFT18GREENINSETZH;
    }
    if (input == Sexy::FONT_DWARVENTODCRAFT18YELLOW) {
        return Sexy::FONT_DWARVENTODCRAFT18YELLOWZH;
    }
    if (input == Sexy::FONT_DWARVENTODCRAFT24) {
        return Sexy::FONT_DWARVENTODCRAFT24ZH;
    }
    if (input == Sexy::FONT_DWARVENTODCRAFT36BRIGHTGREENINSET) {
        return Sexy::FONT_DWARVENTODCRAFT36BRIGHTGREENINSETZH;
    }
    if (input == Sexy::FONT_DWARVENTODCRAFT36GREENINSET) {
        return Sexy::FONT_DWARVENTODCRAFT36GREENINSETZH;
    }
    if (input == Sexy::FONT_HOUSEOFTERROR16) {
        return Sexy::FONT_HOUSEOFTERROR16ZH;
    }
    if (input == Sexy::FONT_HOUSEOFTERROR20) {
        return Sexy::FONT_HOUSEOFTERROR20ZH;
    }
    if (input == Sexy::FONT_HOUSEOFTERROR28) {
        return Sexy::FONT_HOUSEOFTERROR28ZH;
    }
    if (input == Sexy::FONT_PICO129) {
        return Sexy::FONT_PICO129ZH;
    }
    if (input == Sexy::FONT_TINYBOLD) {
        return Sexy::FONT_PIX118BOLDZH;
    }
    return input;
}

inline Sexy::Font* GetSettingsDialogFont() {
    return MapZhFont(Sexy::FONT_DWARVENTODCRAFT18);
}

} // namespace Piston
