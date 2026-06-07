#pragma once
#if SEXY_USE_OPENGL

#include <cstdint>
#include <glad/glad.h>

namespace Sexy {
class GLShader;
}

namespace piston {

enum class UpscaleMode {
    Nearest,
    Bilinear,
    FSR,
};

class Upscaler {
public:
    bool Init(int srcW, int srcH, int dstW, int dstH);
    void Cleanup();

    void Present(GLuint srcFBO, GLuint srcTex, int srcW, int srcH,
                 int dstX, int dstY, int dstW, int dstH);
    void OnResize(int dstW, int dstH);

    void         SetMode(UpscaleMode mode) { mMode = mode; }
    UpscaleMode  GetMode() const           { return mMode; }
    void         SetSharpness(float s);

private:
    void PresentNearest (GLuint srcFBO,                              int dstX, int dstY, int dstW, int dstH);
    void PresentBilinear(GLuint srcFBO,                              int dstX, int dstY, int dstW, int dstH);
    void PresentFSR     (GLuint srcTex, int srcW, int srcH,          int dstX, int dstY, int dstW, int dstH);

    bool InitFSRShaders();
    void RecreateEASUBuffer();
    void RecomputeFSRConstants();

    UpscaleMode      mMode        = UpscaleMode::Bilinear;
    bool             mInitialized = false;

    int mSrcW = 0, mSrcH = 0;
    int mDstW = 0, mDstH = 0;

    // FSR GL resources
    GLuint           mEASUFBO       = 0;
    GLuint           mEASUTexture   = 0;
    GLuint           mFullscreenVAO = 0;

    Sexy::GLShader*  mEASUShader    = nullptr;
    Sexy::GLShader*  mRCASShader    = nullptr;

    // Cached FSR constants (recomputed on resize / sharpness change)
    uint32_t         mEASUCon0[4]   = {};
    uint32_t         mEASUCon1[4]   = {};
    uint32_t         mEASUCon2[4]   = {};
    uint32_t         mEASUCon3[4]   = {};
    uint32_t         mRCASCon[4]    = {};

    float            mSharpness     = 0.2f;
};

} // namespace piston

#endif // SEXY_USE_OPENGL
