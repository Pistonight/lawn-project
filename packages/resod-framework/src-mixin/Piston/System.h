#pragma once

#include <string>

namespace Piston {

enum class Language {
    EN, // English
    ZH  // Chinese
};

class System {
public:
    // singleton no copy/assign
    System(const System&) = delete;
    System& operator=(const System&) = delete;
    static System& Instance() {
        static System gInstance;
        return gInstance;
    }

    static bool IsChinese() {
        return Instance().GetLanguage() == Language::ZH;
    }

    Language GetLanguage() const { return mLanguage; }
    void SetLanguageForNextTime(Language language) const;
    const std::string& GetCurrPath() const { return mCurrPath; }
private:
    System();

    std::string mCurrPath;
    Language mLanguage;

};

}
