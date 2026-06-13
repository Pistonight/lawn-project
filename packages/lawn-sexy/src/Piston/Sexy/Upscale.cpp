#if SEXY_USE_OPENGL

#include "Upscale.h"
// FSR CPU-side constant setup - must come before GL headers to avoid macro conflicts
#include <SDL3/SDL.h>
#include <SexyAppFramework/OpenGL/GLShader.h>
#include <cstdio>
#include <fsr/ffx_a.h>
#include <fsr/ffx_fsr1.h>
#include <fstream>
#include <glad/glad.h>

namespace piston {

void FsrUpscaler::Uninit() {
    mInitialized = false;
    if (mEasuFBO) {
        glDeleteFramebuffers(1, &mEasuFBO);
        mEasuFBO = 0;
    }
    if (mEasuTexture) {
        glDeleteTextures(1, &mEasuTexture);
        mEasuTexture = 0;
    }
    if (mFullscreenVAO) {
        glDeleteVertexArrays(1, &mFullscreenVAO);
        mFullscreenVAO = 0;
    }
    // shaders are uninit-ed when re-init
}

bool FsrUpscaler::Init(int srcW, int srcH, int dstW, int dstH) {
    Uninit();
    // init shaders
    std::string vertex_shader_src = LoadShaderSrc("FsrVertex.glsl");
    std::string easu_shader_src = LoadShaderSrc("FsrEasu_g.glsl");
    std::string rcas_shader_src = LoadShaderSrc("FsrRcas_g.glsl");

    if (!mEasuShader.LoadFromSource(vertex_shader_src, easu_shader_src)) {
        printf("[Piston] FSR EASU Shader failed to load\n");
        return false;
    }
    if (!mRcasShader.LoadFromSource(vertex_shader_src, rcas_shader_src)) {
        printf("[Piston] FSR RCAS Shader failed to load\n");
        return false;
    }
    // init vertex array
    glGenVertexArrays(1, &mFullscreenVAO);

    Reload(srcW, srcH, dstW, dstH);
    mInitialized = true;
    return true;
}

void FsrUpscaler::Reload(int srcW, int srcH, int dstW, int dstH) {
    // recreate buffer/texture
    if (mEasuFBO) {
        glDeleteFramebuffers(1, &mEasuFBO);
        mEasuFBO = 0;
    }
    if (mEasuTexture) {
        glDeleteTextures(1, &mEasuTexture);
        mEasuTexture = 0;
    }

    glGenTextures(1, &mEasuTexture);
    glBindTexture(GL_TEXTURE_2D, mEasuTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, dstW, dstH, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenFramebuffers(1, &mEasuFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, mEasuFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mEasuTexture, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // FsrEasuCon: outAU4 = AU1* (uint32_t*), pass arrays directly
    FsrEasuCon(mEasuCon0, mEasuCon1, mEasuCon2, mEasuCon3, (float)srcW,
               (float)srcH,               // rendered viewport size
               (float)srcW, (float)srcH,  // input texture size
               (float)dstW, (float)dstH); // output size

    FsrRcasCon(mRcasCon, mSharpness);
}

void FsrUpscaler::Present(GLuint srcTex, int srcW, int srcH, int dstX, int dstY, int dstW,
                          int dstH) {
    if (!mInitialized) {
        return;
    }

    glDisable(GL_BLEND);
    glDisable(GL_SCISSOR_TEST);
    glBindVertexArray(mFullscreenVAO);
    glActiveTexture(GL_TEXTURE0);

    // EASU pass: srcTex → mEASUFBO at full dst size
    glBindFramebuffer(GL_FRAMEBUFFER, mEasuFBO);
    glViewport(0, 0, dstW, dstH);
    mEasuShader.Use();
    mEasuShader.SetUniform("uInputTexture", 0);
    glUniform4uiv(glGetUniformLocation(mEasuShader.GetID(), "con0"), 1, mEasuCon0);
    glUniform4uiv(glGetUniformLocation(mEasuShader.GetID(), "con1"), 1, mEasuCon1);
    glUniform4uiv(glGetUniformLocation(mEasuShader.GetID(), "con2"), 1, mEasuCon2);
    glUniform4uiv(glGetUniformLocation(mEasuShader.GetID(), "con3"), 1, mEasuCon3);
    glBindTexture(GL_TEXTURE_2D, srcTex);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // RCAS pass: mEASUTexture → default FB at presentation rect
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(dstX, dstY, dstW, dstH);
    mRcasShader.Use();
    mRcasShader.SetUniform("uInputTexture", 0);
    glUniform2i(glGetUniformLocation(mRcasShader.GetID(), "uViewportOffset"), dstX, dstY);
    glUniform4uiv(glGetUniformLocation(mRcasShader.GetID(), "con"), 1, mRcasCon);
    glBindTexture(GL_TEXTURE_2D, mEasuTexture);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glBindVertexArray(0);
}

void Upscaler::Uninit() {
    mFsr.Uninit();
}

void Upscaler::Init(int srcW, int srcH, int dstW, int dstH) {
    mSrcW = srcW;
    mSrcH = srcH;
    mDstW = dstW;
    mDstH = dstH;
    if (!mFsr.Init(srcW, srcH, dstW, dstH)) {
        printf("[Piston] FSR Upscaler failed to init\n");
    }
}
bool Upscaler::IsModeEnabled(UpscaleMode mode) {
    switch (mode) {
    case UpscaleMode::Nearest:
    case UpscaleMode::Bilinear:
        return true;
    case UpscaleMode::FSR:
        return mFsr.IsInitialized();
    default:
        return false;
    }
    return false;
}

void Upscaler::OnResize(int srcW, int srcH, int dstW, int dstH) {
    if (srcW == mSrcW && srcH == mSrcH && dstW == mDstW && dstH == mDstH) {
        return;
    }
    mSrcW = srcW;
    mSrcH = srcH;
    mDstW = dstW;
    mDstH = dstH;
    mFsr.Reload(mSrcW, mSrcH, mDstW, mDstH);
}

std::string LoadShaderSrc(const char* file) {
    printf("[Piston] Loading shader %s\n", file);
    std::string path = SDL_GetBasePath();
    path += "shaders/";
    path += file;

    std::ifstream file_stream(path, std::ios::binary);
    return std::string((std::istreambuf_iterator<char>(file_stream)),
                       std::istreambuf_iterator<char>());
}

void Upscaler::Present(GLuint srcFBO, GLuint srcTex, int dstX, int dstY) {
    switch (mMode) {
    case UpscaleMode::Nearest:
        PresentNearest(srcFBO, dstX, dstY, mDstW, mDstH);
        break;
    case UpscaleMode::Bilinear:
        PresentBilinear(srcFBO, dstX, dstY, mDstW, mDstH);
        break;
    case UpscaleMode::FSR:
        mFsr.Present(srcTex, mSrcW, mSrcH, dstX, dstY, mDstW, mDstH);
        break;
    default:
        break;
    }
}

void Upscaler::PresentNearest(GLuint srcFBO, int dstX, int dstY, int dstW, int dstH) {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, srcFBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, mSrcW, mSrcH, dstX, dstY, dstX + dstW, dstY + dstH, GL_COLOR_BUFFER_BIT,
                      GL_NEAREST);
}

void Upscaler::PresentBilinear(GLuint srcFBO, int dstX, int dstY, int dstW, int dstH) {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, srcFBO);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, mSrcW, mSrcH, dstX, dstY, dstX + dstW, dstY + dstH, GL_COLOR_BUFFER_BIT,
                      GL_LINEAR);
}

} // namespace piston

#endif // SEXY_USE_OPENGL
