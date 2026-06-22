#include <SexyAppFramework/Common.h>
#include <LawnApp/LawnApp.h>
#include <PakLib/PakInterface.h>

#include <Piston/Init.h>
#include <Piston/System.h>

namespace Piston {

void InitMod(LawnApp& app) {
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

}
