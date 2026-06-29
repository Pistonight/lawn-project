#include <LawnApp/LawnApp.h>
#include <LawnApp/Resources.h>
#include <PakLib/PakInterface.h>
#include <Sexy.TodLib/TodStringFile.h>
#include <SexyAppFramework/Common.h>
#include <print>

#include <Piston/Font.h>
#include <Piston/Init.h>
#include <Piston/System.h>

namespace Piston {

void InitMain(LawnApp& app) {
    std::println("Piston::InitMain");
    // this will put data at Piston/PlantsVsZombies
    app.mFullCompanyName = "Piston";

    auto& system = Piston::System::Instance();
    Sexy::SetAppDataFolder(system.GetCurrPath());
    // load our multi-lang pak files
    gPakInterface->AddPakFile("shared.pak");
    if (system.IsChinese()) {
        gPakInterface->AddPakFile("mainzh.pak");
    } else {
        gPakInterface->AddPakFile("mainen.pak");
    }
}

void InitLoadingScreen(LawnApp& app) {
    std::println("Piston::InitLoadingScreen");
    TodStringListLoad("properties/ExtraLawnStrings.txt");
    TodStringListLoad("properties/ModStrings.txt");
}

void InitLoadingMain(LawnApp& app) {
    std::println("Piston::InitLoadingMain");
    app.LoadGroup("LoadingFontsZHShare", 10);
    if (Piston::System::Instance().IsChinese()) {
        app.LoadGroup("LoadingFontsZH", 10);
    }
}

} // namespace Piston
