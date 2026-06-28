#pragma once

#include <LawnApp/LawnApp.h>
#include <Piston/RendererMixinState.h>

namespace Piston {

class LawnAppMixin {
public:
    LawnAppMixin(LawnApp* app) : mApp(app) {}

    void WriteToRegistry();
    void ReadFromRegistry();
    OpenGLRendererMixinState& GetRendererMixin();

private:
    LawnApp* mApp;
};

} // namespace Piston
