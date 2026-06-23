#include <Piston/Font.h>
#include <LawnApp/Resources.h>
#include <SexyAppFramework/SysFont.h>


namespace Piston::FontLoader {

void InitLoaderBarFonts() {
    Sexy::FONT_BRIANNETOD16 = new Sexy::SysFont(
        "FZKaTong-M19S",
        12 /* point */,
        true /* bold */,
        false /* italic */,
        false /* underline */
    );
}

void InitLoadingFonts() {
    Sexy::FONT_BRIANNETOD12 = new Sexy::SysFont(
        "FZKaTong-M19S",
        10 /* point */,
        false /* bold */,
        false /* italic */,
        false /* underline */
    );

}

}
