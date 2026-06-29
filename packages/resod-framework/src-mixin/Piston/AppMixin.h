#pragma once

#include <Piston/RendererMixinState.h>

class LawnApp;

namespace Piston {

class LawnAppMixin {
public:
    LawnAppMixin(LawnApp* app) : mApp(app) {}

    void WriteToRegistry();
    void ReadFromRegistry();
    OpenGLRendererMixinState& GetRendererMixin();
    void SetDebugWindowEnabled(bool enabled);
    bool IsDebugWindowEnabled() const;

private:
    LawnApp* mApp;
};

} // namespace Piston
