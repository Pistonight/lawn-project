#include "System.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <print>
#include <SDL3/SDL_filesystem.h>

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
        std::println("Failed to read language, assuming English");
        return Language::EN;
    }
}

}

System::System() {
    mCurrPath = SDL_GetBasePath();
    std::println("Curr path is {}", mCurrPath);
    mLanguage = LoadCurrentLanguage(mCurrPath);
}

void System::SetLanguageForNextTime(Language language) const {
    try {
        auto langPath = std::filesystem::path(mCurrPath) / ".language";
        std::ofstream langFile(langPath, std::ios::trunc);
        if (!langFile) {
            return;
        }
        langFile << (language == Language::ZH ? "zh" : "en") << "\n";
    } catch (...) {
        std::println("Failed to write language");
    }
}

}
