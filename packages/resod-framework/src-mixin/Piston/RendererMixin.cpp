#include <LawnApp/LawnApp.h>
#include <Piston/RendererMixin.h>

namespace Piston {

void OpenGLRendererMixin::Init() {
    auto& fsrUpscaler = mRenderer->mPistonMixin.mFsrUpscaler;
    int width = mRenderer->mWidth;
    int height = mRenderer->mHeight;
    fsrUpscaler.Init(width, height, width, height);
}

void OpenGLRendererMixin::Cleanup() {
    mRenderer->mPistonMixin.mFsrUpscaler.Uninit();
}

bool OpenGLRendererMixin::Redraw() {
    auto& appMixin = ((LawnApp*)(mRenderer->mApp))->mPistonMixin;
    switch (appMixin.mUpscaleMode) {
    case UpscaleMode::None:
        return false;
    case UpscaleMode::Fsr: {
        auto& fsrUpscaler = mRenderer->mPistonMixin.mFsrUpscaler;
        if (!fsrUpscaler.IsInitialized()) {
            return false;
        }
        fsrUpscaler.Present(mRenderer->mFBOTexture, mRenderer->mPresentationRect.mX,
                            mRenderer->mPresentationRect.mY);
        return true;
    }
    default:
        return false;
    }
}

void OpenGLRendererMixin::UpdateViewport(int viewportW, int viewportH) {
    auto& fsrUpscaler = mRenderer->mPistonMixin.mFsrUpscaler;
    fsrUpscaler.Reload(mRenderer->mWidth, mRenderer->mHeight, viewportW, viewportH);
}

} // namespace Piston
