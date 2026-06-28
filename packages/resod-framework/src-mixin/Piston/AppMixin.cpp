#include <Piston/AppMixin.h>
#include <SexyAppFramework/OpenGL/OpenGLRenderer.h>

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

} // namespace Piston
