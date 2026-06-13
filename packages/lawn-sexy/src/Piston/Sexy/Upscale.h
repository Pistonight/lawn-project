#pragma once
#if SEXY_USE_OPENGL

// #include <memory>
#include <SexyAppFramework/OpenGL/GLShader.h>
#include <cstdint>
#include <glad/glad.h>
#include <string>

namespace piston {

enum class UpscaleMode { Nearest = 0, Bilinear = 1, FSR = 2, NumUpscalers };

// using Init/Uninit instead of ctor/dtor to better align with what PVZ does

class FsrUpscaler {
public:
    void Uninit();
    bool Init(int srcW, int srcH, int dstW, int dstH);
    void Reload(int srcW, int srcH, int dstW, int dstH);
    bool IsInitialized() const { return mInitialized; }
    void Present(GLuint srcTex, int srcW, int srcH, int dstX, int dstY, int dstW, int dstH);

private:
    bool mInitialized = false;
    // FSR GL resources
    GLuint mEasuFBO = 0;
    GLuint mEasuTexture = 0;
    GLuint mFullscreenVAO = 0; // vertex array

    Sexy::GLShader mEasuShader;
    Sexy::GLShader mRcasShader;

    // Cached FSR constants (recomputed on resize / sharpness change)
    uint32_t mEasuCon0[4] = {};
    uint32_t mEasuCon1[4] = {};
    uint32_t mEasuCon2[4] = {};
    uint32_t mEasuCon3[4] = {};
    uint32_t mRcasCon[4] = {};

    float mSharpness = 0.87f;
};

class Upscaler {
public:
    void Uninit();
    void Init(int srcW, int srcH, int dstW, int dstH);
    bool IsModeEnabled(UpscaleMode mode);
    void OnResize(int srcW, int srcH, int dstW, int dstH);

    void Present(GLuint srcFBO, GLuint srcTex, int dstX, int dstY);

    void SetMode(UpscaleMode mode) {
        if (!IsModeEnabled(mode)) {
            return;
        }
        mMode = mode;
    }
    UpscaleMode GetMode() const { return mMode; }

private:
    void PresentNearest(GLuint srcFBO, int dstX, int dstY, int dstW, int dstH);
    void PresentBilinear(GLuint srcFBO, int dstX, int dstY, int dstW, int dstH);

    UpscaleMode mMode = UpscaleMode::FSR;
    // track current parameters to avoid recreate when not needed
    int mSrcW = 0;
    int mSrcH = 0;
    int mDstW = 0;
    int mDstH = 0;

    FsrUpscaler mFsr;
};

std::string LoadShaderSrc(const char* file);

} // namespace piston

#endif // SEXY_USE_OPENGL
