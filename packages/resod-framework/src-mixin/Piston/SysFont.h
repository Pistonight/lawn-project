#pragma once

#include <vector>
#include <string>
#include <optional>
#include <memory>
#include <freetype/freetype.h>
#include <SexyAppFramework/SysFont.h>

namespace Sexy {
class SexyAppBase;
}

namespace Piston::SysFont {

std::shared_ptr<Sexy::TrueTypeData> MakeTrueTypeData(
    Sexy::SexyAppBase& app, const std::string& fontFace,
    int theSize, bool theBold
);

std::optional<std::vector<unsigned char>> LoadFontFile(const std::string& fontFileName);

}
