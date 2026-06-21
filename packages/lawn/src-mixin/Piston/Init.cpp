#include <SDL3/SDL_filesystem.h>

#include <SexyAppFramework/Common.h>
#include <LawnApp/LawnApp.h>
#include <PakLib/PakInterface.h>

#include <Piston/Init.h>

namespace Piston {

void InitMod(LawnApp& app) {
    // load our multi-lang pak files
    gPakInterface->AddPakFile("shared.pak");
    gPakInterface->AddPakFile("mainen.pak");
    // this will put data at Piston/PlantsVsZombies
    app.mFullCompanyName = "Piston";
    std::string appPath = SDL_GetBasePath();
    Sexy::SetAppDataFolder(appPath);
}

}
