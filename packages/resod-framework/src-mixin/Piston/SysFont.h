#pragma once

#include <vector>
#include <string>
#include <optional>
#include <freetype/freetype.h>

namespace Piston::SysFont {


class FontObj {
public:
    FontObj(FT_Library library, const std::string& fontFace);
    ~FontObj();

    // get a copy of the pointer
    std::optional<FT_Face> GetFontFace() const { return mFontFace; }
private:
    // nullopt means failed to load the font
    std::optional<FT_Face> mFontFace{};
    // for in-memory fonts, the font file must be kept alive
    std::optional<std::vector<unsigned char>> mFontBytes{};
};


std::optional<std::vector<unsigned char>> LoadFontFile(const std::string& fontFileName);
}
