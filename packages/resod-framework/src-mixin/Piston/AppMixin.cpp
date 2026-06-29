#include <LawnApp/LawnApp.h>
#include <SexyAppFramework/OpenGL/OpenGLRenderer.h>

#include <Lawn/ResoddedFramework/DebuggerWindow.h>
#include <Piston/AppMixin.h>

namespace Piston {

void LawnAppMixin::WriteToRegistry() {
    auto& mixin = mApp->mPistonMixin;
    mApp->RegistryWriteInteger("Piston_UpscaleMode", static_cast<int>(mixin.mUpscaleMode));
}

void LawnAppMixin::ReadFromRegistry() {
    auto& mixin = mApp->mPistonMixin;
    int upscaleMode{};
    if (mApp->RegistryReadInteger("Piston_UpscaleMode", &upscaleMode)) {
        if (upscaleMode >= 0 && upscaleMode < static_cast<int>(UpscaleMode::LENGTH)) {
            mixin.mUpscaleMode = static_cast<UpscaleMode>(upscaleMode);
        }
    }
}

OpenGLRendererMixinState& LawnAppMixin::GetRendererMixin() {
    auto* renderer = reinterpret_cast<Sexy::OpenGLRenderer*>(mApp->mRenderer);
    return renderer->mPistonMixin;
}

void LawnAppMixin::SetDebugWindowEnabled(bool enabled) {
    bool enabledNow = IsDebugWindowEnabled();
    if (enabledNow == enabled) {
        return;
    }
    if (enabled) {
        if (!mApp->mDebugWindow) {
            mApp->mDebugWindow = new DebuggerWindow(mApp);
        } else {
            mApp->mDebugWindow->mEnabled = true;
        }
    } else {
        if (mApp->mDebugWindow) {
            mApp->mDebugWindow->mEnabled = false;
        }
    }
    mApp->mDebuggerEnabled = enabled;
}

bool LawnAppMixin::IsDebugWindowEnabled() const {
    auto* debugWindow = mApp->mDebugWindow;
    if (!debugWindow) {
        return false;
    }
    return debugWindow->mEnabled && mApp->mDebuggerEnabled;
}

} // namespace Piston
