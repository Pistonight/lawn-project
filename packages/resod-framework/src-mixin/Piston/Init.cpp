#include <print>
#include <SexyAppFramework/Common.h>
#include <LawnApp/LawnApp.h>
#include <PakLib/PakInterface.h>

#include <Piston/Init.h>
#include <Piston/Font.h>
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

void InitLoaderScreen(LawnApp&) {
    std::println("Piston::InitLoaderScreen");
    FontLoader::InitLoaderBarFonts();
}

void InitPostLoad(LawnApp&) {
    std::println("Piston::PostLoad");
    FontLoader::InitLoadingFonts();
}

}
