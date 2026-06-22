#pragma once

#include <LawnApp/Resources.h>

#include <Piston/System.h>

namespace Sexy { 
class Font;
} // namespace Sexy

namespace Piston {

// initialize fonts after they are loaded
void InitFont();

inline Sexy::Font* GetSettingsDialogFont() {
    // if (System::IsChinese()) {
    // }
    return Sexy::FONT_BRIANNETOD12;
}


}
