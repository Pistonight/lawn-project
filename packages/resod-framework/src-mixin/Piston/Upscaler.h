#pragma once

#include <SexyAppFramework/OpenGL/GLShader.h>
#include <glad/glad.h>
#include <memory>
#include <optional>

namespace Piston {

enum class UpscaleMode { None, Fsr, LENGTH };

class FsrUpscaler {
public:
    void Uninit();
    bool Init(int srcW, int srcH, int dstW, int dstH);
    void Reload(int srcW, int srcH, int dstW, int dstH);
    bool IsInitialized() const { return mInitialized; }
    void Present(GLuint srcTex, int dstX, int dstY);

private:
    bool mInitialized{};

    // initialized size
    int mSrcW{};
    int mSrcH{};
    int mDstW{};
    int mDstH{};

    // FSR GL resources
    GLuint mEasuFBO{};
    GLuint mEasuTexture{};
    GLuint mFullscreenVAO{}; // vertex array

    // using pointer because the object is RAII container for the compiled shader program
    // it should be minimal perf difference
    std::unique_ptr<Sexy::GLShader> mEasuShader;
    std::unique_ptr<Sexy::GLShader> mRcasShader;

    // Cached FSR constants (recomputed on resize / sharpness change)
    uint32_t mEasuCon0[4]{};
    uint32_t mEasuCon1[4]{};
    uint32_t mEasuCon2[4]{};
    uint32_t mEasuCon3[4]{};
    uint32_t mRcasCon[4]{};

    float mSharpness = 0.87f;
};

std::optional<std::string> LoadShaderSrc(const char* file);

} // namespace Piston
