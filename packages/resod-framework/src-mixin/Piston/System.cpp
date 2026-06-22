#include "System.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <SDL3/SDL_filesystem.h>
#include <sstream>

namespace Piston {

namespace {

Language LoadCurrentLanguage(const std::string& currPath) {
    try {
        auto langPath = std::filesystem::path(currPath) / ".language";
        std::ifstream langFile(langPath);
        if (!langFile) {
            return Language::EN;
        }
        std::ostringstream contentStream;
        contentStream << langFile.rdbuf();
        std::string content = contentStream.str();
        if (content.starts_with("zh")) {
            return Language::ZH;
        }

        return Language::EN;
    } catch (...) {
        std::cout << "Failed to read language, assuming English" << std::endl;
        return Language::EN;
    }
}

}

System::System() {
    mCurrPath = SDL_GetBasePath();
    mLanguage = LoadCurrentLanguage(mCurrPath);
}

}
