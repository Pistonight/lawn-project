#pragma once

#include <SexyAppFramework/OpenGL/OpenGLRenderer.h>

namespace Piston {

class OpenGLRendererMixin {
public:
    OpenGLRendererMixin(Sexy::OpenGLRenderer* renderer) : mRenderer(renderer) {}

    void Init();
    void Cleanup();
    bool Redraw();
    void UpdateViewport(int viewportW, int viewportH);

private:
    Sexy::OpenGLRenderer* mRenderer;
};

} // namespace Piston
