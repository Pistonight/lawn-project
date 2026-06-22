#include <freetype/freetype.h>
#include <print>
#include <format>
#include <PakLib/PakInterface.h>

#include <Piston/SysFont.h>

namespace Piston::SysFont {

FontObj::FontObj(FT_Library library, const std::string& fontFace) {
    auto bytes = LoadFontFile(fontFace);
    if (bytes) {
        // move it first for face object to reference it
        mFontBytes = std::move(bytes);
        // instantiate in-memory font file
        FT_Face face{};
        FT_Error error = FT_New_Memory_Face(library, mFontBytes->data(), mFontBytes->size(),
                                            0, &face);
        if (error) {
            std::println("failed to instantiate memory face: FT_Error {}!", error);
            return;
        }
        mFontFace = face;
        return;
    }

    // fallback from creating from system font
    FT_Face face{};
    FT_Error error = FT_New_Face(library, ("C:/Windows/Fonts/" + fontFace + ".ttf").c_str(),
                                 0, &face);
    if (error) {
        std::println("failed to instantiate face from system font: FT_Error {}!", error);
        return;
    }
    mFontFace = face;
}

FontObj::~FontObj() {
    if (mFontFace) {
        FT_Error error = FT_Done_Face(*mFontFace);
        std::println("failed to destroy face: FT_Error {}!", error);
    }
    mFontBytes.reset();
}

std::optional<std::vector<unsigned char>> LoadFontFile(const std::string& fontFace) {
    std::string fontFileName = fontFace + ".ttf";
    PFILE* pFile = p_fopen(fontFileName.c_str(), "rb");
    if (pFile == nullptr) {
        std::println("failed to load font file '{}' from pak!", fontFileName);
        return std::nullopt;
    }
    p_fseek(pFile, 0, SEEK_END);
    auto aSize = p_ftell(pFile);
    p_fseek(pFile, 0, SEEK_SET);

    if (aSize <= 0) {
        std::println("failed to load font file '{}' from pak: empty file!", fontFileName);
        p_fclose(pFile);
        return std::nullopt;
    }

    std::vector<unsigned char> buf;
    buf.reserve(aSize);

    std::size_t aReadSize = p_fread(buf.data(), sizeof(unsigned char), aSize, pFile);
    buf.resize(aReadSize);
    std::println("loaded font file '{}'", fontFileName);
    p_fclose(pFile);

    return std::move(buf);
}

}
