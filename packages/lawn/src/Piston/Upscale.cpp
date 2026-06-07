// #if SEXY_USE_OPENGL
//
// // FSR CPU-side constant setup — must come before GL headers to avoid macro conflicts
// #define A_CPU 1
// #include "../../lib/FidelityFX-FSR/ffx_a.h"
// #include "../../lib/FidelityFX-FSR/ffx_fsr1.h"
//
// #include "Upscale.h"
// #include "Shaders.h"
// #include <glad/glad.h>
//
// namespace piston {
//
// // ---------------------------------------------------------------------------
// // Init / Cleanup
// // ---------------------------------------------------------------------------
//
// bool Upscaler::Init(int srcW, int srcH, int dstW, int dstH)
// {
//     mSrcW = srcW; mSrcH = srcH;
//     mDstW = dstW; mDstH = dstH;
//
//     glGenVertexArrays(1, &mFullscreenVAO);
//
//     if (!InitFSRShaders())
//         return false;
//
//     RecreateEASUBuffer();
//     RecomputeFSRConstants();
//
//     mInitialized = true;
//     return true;
// }
//
// void Upscaler::Cleanup()
// {
//     if (!mInitialized) return;
//
//     if (mEASUFBO)       { glDeleteFramebuffers(1,  &mEASUFBO);       mEASUFBO       = 0; }
//     if (mEASUTexture)   { glDeleteTextures(1,       &mEASUTexture);   mEASUTexture   = 0; }
//     if (mFullscreenVAO) { glDeleteVertexArrays(1,   &mFullscreenVAO); mFullscreenVAO = 0; }
//
//     delete mEASUShader; mEASUShader = nullptr;
//     delete mRCASShader; mRCASShader = nullptr;
//
//     mInitialized = false;
// }
//
// // ---------------------------------------------------------------------------
// // Internal helpers
// // ---------------------------------------------------------------------------
//
// bool Upscaler::InitFSRShaders()
// {
//     mEASUShader = new Sexy::GLShader();
//     if (!mEASUShader->LoadFromSource(GetFsrVertexShaderSrc(), GetFsrEasuShaderSrc()))
//     {
//         delete mEASUShader; mEASUShader = nullptr;
//         return false;
//     }
//
//     mRCASShader = new Sexy::GLShader();
//     if (!mRCASShader->LoadFromSource(GetFsrVertexShaderSrc(), GetFsrRcasShaderSrc()))
//     {
//         delete mRCASShader; mRCASShader = nullptr;
//         return false;
//     }
//
//     return true;
// }
//
// void Upscaler::RecreateEASUBuffer()
// {
//     if (mEASUFBO)
//     {
//         glDeleteFramebuffers(1, &mEASUFBO);
//         glDeleteTextures(1, &mEASUTexture);
//     }
//
//     glGenTextures(1, &mEASUTexture);
//     glBindTexture(GL_TEXTURE_2D, mEASUTexture);
//     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, mDstW, mDstH, 0, GL_RGBA, GL_FLOAT, nullptr);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//     glBindTexture(GL_TEXTURE_2D, 0);
//
//     glGenFramebuffers(1, &mEASUFBO);
//     glBindFramebuffer(GL_FRAMEBUFFER, mEASUFBO);
//     glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mEASUTexture, 0);
//     glBindFramebuffer(GL_FRAMEBUFFER, 0);
// }
//
// void Upscaler::RecomputeFSRConstants()
// {
//     // FsrEasuCon: outAU4 = AU1* (uint32_t*), pass arrays directly
//     FsrEasuCon(
//         mEASUCon0, mEASUCon1, mEASUCon2, mEASUCon3,
//         (float)mSrcW, (float)mSrcH,   // rendered viewport size
//         (float)mSrcW, (float)mSrcH,   // input texture size (same — no dynamic res)
//         (float)mDstW, (float)mDstH);  // output size
//
//     FsrRcasCon(mRCASCon, mSharpness);
// }
//
// // ---------------------------------------------------------------------------
// // Public interface
// // ---------------------------------------------------------------------------
//
// void Upscaler::SetSharpness(float s)
// {
//     mSharpness = s;
//     if (mInitialized)
//         FsrRcasCon(mRCASCon, mSharpness);
// }
//
// void Upscaler::OnResize(int dstW, int dstH)
// {
//     if (!mInitialized || (dstW == mDstW && dstH == mDstH)) return;
//     mDstW = dstW;
//     mDstH = dstH;
//     RecreateEASUBuffer();
//     RecomputeFSRConstants();
// }
//
// void Upscaler::Present(GLuint srcFBO, GLuint srcTex, int srcW, int srcH,
//                        int dstX, int dstY, int dstW, int dstH)
// {
//     switch (mMode)
//     {
//     case UpscaleMode::Nearest:  PresentNearest (srcFBO,           dstX, dstY, dstW, dstH); break;
//     case UpscaleMode::Bilinear: PresentBilinear(srcFBO,           dstX, dstY, dstW, dstH); break;
//     case UpscaleMode::FSR:      PresentFSR     (srcTex, srcW, srcH, dstX, dstY, dstW, dstH); break;
//     }
// }
//
// // ---------------------------------------------------------------------------
// // Present modes
// // ---------------------------------------------------------------------------
//
// void Upscaler::PresentNearest(GLuint srcFBO, int dstX, int dstY, int dstW, int dstH)
// {
//     glBindFramebuffer(GL_READ_FRAMEBUFFER, srcFBO);
//     glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
//     glBlitFramebuffer(0, 0, mSrcW, mSrcH,
//                       dstX, dstY, dstX + dstW, dstY + dstH,
//                       GL_COLOR_BUFFER_BIT, GL_NEAREST);
// }
//
// void Upscaler::PresentBilinear(GLuint srcFBO, int dstX, int dstY, int dstW, int dstH)
// {
//     glBindFramebuffer(GL_READ_FRAMEBUFFER, srcFBO);
//     glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
//     glBlitFramebuffer(0, 0, mSrcW, mSrcH,
//                       dstX, dstY, dstX + dstW, dstY + dstH,
//                       GL_COLOR_BUFFER_BIT, GL_LINEAR);
// }
//
// void Upscaler::PresentFSR(GLuint srcTex, int srcW, int srcH,
//                            int dstX, int dstY, int dstW, int dstH)
// {
//     if (!mEASUShader || !mRCASShader || !mEASUFBO) return;
//
//     glDisable(GL_BLEND);
//     glDisable(GL_SCISSOR_TEST);
//     glBindVertexArray(mFullscreenVAO);
//     glActiveTexture(GL_TEXTURE0);
//
//     // EASU pass: srcTex → mEASUFBO at full dst size
//     glBindFramebuffer(GL_FRAMEBUFFER, mEASUFBO);
//     glViewport(0, 0, mDstW, mDstH);
//     mEASUShader->Use();
//     mEASUShader->SetUniform("uInputTexture", 0);
//     glUniform4uiv(glGetUniformLocation(mEASUShader->GetID(), "con0"), 1, mEASUCon0);
//     glUniform4uiv(glGetUniformLocation(mEASUShader->GetID(), "con1"), 1, mEASUCon1);
//     glUniform4uiv(glGetUniformLocation(mEASUShader->GetID(), "con2"), 1, mEASUCon2);
//     glUniform4uiv(glGetUniformLocation(mEASUShader->GetID(), "con3"), 1, mEASUCon3);
//     glBindTexture(GL_TEXTURE_2D, srcTex);
//     glDrawArrays(GL_TRIANGLES, 0, 3);
//
//     // RCAS pass: mEASUTexture → default FB at presentation rect
//     glBindFramebuffer(GL_FRAMEBUFFER, 0);
//     glViewport(dstX, dstY, dstW, dstH);
//     mRCASShader->Use();
//     mRCASShader->SetUniform("uInputTexture", 0);
//     glUniform4uiv(glGetUniformLocation(mRCASShader->GetID(), "con"), 1, mRCASCon);
//     glBindTexture(GL_TEXTURE_2D, mEASUTexture);
//     glDrawArrays(GL_TRIANGLES, 0, 3);
//
//     glBindVertexArray(0);
// }
//
// } // namespace piston
//
// #endif // SEXY_USE_OPENGL
