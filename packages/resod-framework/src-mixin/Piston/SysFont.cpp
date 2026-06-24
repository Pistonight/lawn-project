#include <freetype/freetype.h>
#include <print>
#include <format>
#include <PakLib/PakInterface.h>
#include <SexyAppFramework/SexyAppBase.h>

#include <Piston/SysFont.h>

namespace Piston::SysFont {
std::shared_ptr<Sexy::TrueTypeData> MakeTrueTypeData(
    Sexy::SexyAppBase& app, const std::string& fontFace,
    int theSize, bool theBold
) {
    auto bytes = LoadFontFile(fontFace);
    if (!bytes) {
        // if failed to load font fallback from creating from system font
        FT_Face face{};
        FT_Error error = FT_New_Face(app.mFreeTypeLib, ("C:/Windows/Fonts/" + fontFace + ".ttf").c_str(),
                                     0, &face);
        if (error) {
            std::println("failed to instantiate face from system font: FT_Error {}!", error);
            return nullptr;
        }

        return std::make_shared<Sexy::TrueTypeData>(&app, face, theSize, theBold);
    }

    // for in-memory buffer, the data is backed by a buffer, so
    // we have to move the buffer to the final memory location first
    // create the data with nullptr which does not initialize it yet
    auto data = std::make_shared<Sexy::TrueTypeData>(&app, nullptr, theSize, theBold);
    // load the font file bytes
    data->mFontBytes = std::move(*bytes);
    // instantiate in-memory font file
    FT_Face face{};
    FT_Error error = FT_New_Memory_Face(app.mFreeTypeLib, 
                                        data->mFontBytes.data(), data->mFontBytes.size(),
                                            0, &face);
    if (error) {
        std::println("failed to instantiate memory face: FT_Error {}!", error);
        return nullptr;
    }
    // attach the face pointer to data and initialize
    data->mFace = face;
    data->Init();
    return data;
}

std::optional<std::vector<unsigned char>> LoadFontFile(const std::string& fontFace) {
    std::string fontFileName = "fonts/" + fontFace + ".ttf";
    PFILE* pFile = p_fopen(fontFileName.c_str(), "rb");
    if (pFile == nullptr) {
        std::println("failed to open font file '{}' from pak!", fontFileName);
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
    buf.resize(aSize);

    std::size_t aReadSize = p_fread(buf.data(), sizeof(unsigned char), aSize, pFile);
    buf.resize(aReadSize);
    std::println("loaded font file '{}'", fontFileName);
    p_fclose(pFile);

    return std::move(buf);
}

}
