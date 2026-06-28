#include <PakLib/PakInterface.h>
#include <Piston/Upscaler.h>
#include <print>

#ifndef A_CPU // Include FSR in CPU mode
#define A_CPU
#endif
#include <fsr/ffx_a.h>
#include <fsr/ffx_fsr1.h>

namespace Piston {

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
    mEasuShader.reset();
    mRcasShader.reset();
}

bool FsrUpscaler::Init(int srcW, int srcH, int dstW, int dstH) {
    Uninit();
    // init shaders
    auto vertexShaderSrc = LoadShaderSrc("FsrVertex.glsl");
    if (!vertexShaderSrc) {
        return false;
    }
    auto easuShaderSrc = LoadShaderSrc("FsrEasu_g.glsl");
    if (!easuShaderSrc) {
        return false;
    }
    auto rcasShaderSrc = LoadShaderSrc("FsrRcas_g.glsl");
    if (!easuShaderSrc) {
        return false;
    }

    auto easuShader = std::make_unique<Sexy::GLShader>();
    if (!easuShader->LoadFromSource(*vertexShaderSrc, *easuShaderSrc)) {
        std::println("[Piston] FSR EASU shader failed to load");
        return false;
    }
    std::println("[Piston] FSR EASU shader loaded");
    mEasuShader = std::move(easuShader);

    auto rcasShader = std::make_unique<Sexy::GLShader>();
    if (!rcasShader->LoadFromSource(*vertexShaderSrc, *rcasShaderSrc)) {
        std::println("[Piston] FSR RCAS shader failed to load");
        return false;
    }
    std::println("[Piston] FSR RCAS shader loaded");
    mRcasShader = std::move(rcasShader);

    // init vertex array
    glGenVertexArrays(1, &mFullscreenVAO);

    Reload(srcW, srcH, dstW, dstH);
    mInitialized = true;
    std::println("[Piston] FSR upscaler loaded");
    return true;
}

void FsrUpscaler::Reload(int srcW, int srcH, int dstW, int dstH) {
    if (!mInitialized) {
        return;
    }
    if (srcW == mSrcW && srcH == mSrcH && dstW == mDstW && dstH == mDstH) {
        return;
    }
    mSrcW = srcW;
    mSrcH = srcH;
    mDstW = dstW;
    mDstH = dstH;
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
    std::println("[Piston] FSR upscaler resized: {}x{}", dstW, dstH);
}

void FsrUpscaler::Present(GLuint srcTex, int dstX, int dstY) {
    if (!mInitialized) {
        return;
    }

    glDisable(GL_BLEND);
    glDisable(GL_SCISSOR_TEST);
    glBindVertexArray(mFullscreenVAO);
    glActiveTexture(GL_TEXTURE0);

    // EASU pass: srcTex → mEASUFBO at full dst size
    glBindFramebuffer(GL_FRAMEBUFFER, mEasuFBO);
    glViewport(0, 0, mDstW, mDstH);
    mEasuShader->Use();
    mEasuShader->SetUniform("uInputTexture", 0);
    auto easuId = mEasuShader->GetID();
    glUniform4uiv(glGetUniformLocation(easuId, "con0"), 1, mEasuCon0);
    glUniform4uiv(glGetUniformLocation(easuId, "con1"), 1, mEasuCon1);
    glUniform4uiv(glGetUniformLocation(easuId, "con2"), 1, mEasuCon2);
    glUniform4uiv(glGetUniformLocation(easuId, "con3"), 1, mEasuCon3);
    glBindTexture(GL_TEXTURE_2D, srcTex);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    // RCAS pass: mEASUTexture → default FB at presentation rect
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(dstX, dstY, mDstW, mDstH);
    mRcasShader->Use();
    mRcasShader->SetUniform("uInputTexture", 0);
    auto rcasId = mRcasShader->GetID();
    glUniform2i(glGetUniformLocation(rcasId, "uViewportOffset"), dstX, dstY);
    glUniform4uiv(glGetUniformLocation(rcasId, "con"), 1, mRcasCon);
    glBindTexture(GL_TEXTURE_2D, mEasuTexture);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glBindVertexArray(0);
}

std::optional<std::string> LoadShaderSrc(const char* file) {
    std::println("[Piston] Loading shader '{}'", file);

    std::string fullPath("shaders/");
    fullPath += file;
    PFILE* pfile = p_fopen(fullPath.c_str(), "rb");
    if (!pfile) {
        std::println("[Piston] Failed to load shader: file not found");
        return std::nullopt;
    }

    p_fseek(pfile, 0, SEEK_END);
    auto aSize = p_ftell(pfile);
    p_fseek(pfile, 0, SEEK_SET);
    std::string aBytes;
    aBytes.resize(aSize);
    aSize = p_fread(&aBytes[0], sizeof(char), aSize, pfile);
    if (aSize <= 0) {
        std::println("[Piston] Failed to load shader: error while reading");
        p_fclose(pfile);
        return std::nullopt;
    }
    aBytes.resize(aSize);
    return std::move(aBytes);
}

} // namespace Piston
