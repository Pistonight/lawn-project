#version 400 core
#define A_GPU
#define A_GLSL
#TXTPP#include ../../lib/FidelityFX-FSR/include/ffx_a.h
#define FSR_EASU_F 1
#TXTPP#include ../../lib/FidelityFX-FSR/include/ffx_fsr1.h

uniform sampler2D uInputTexture;
uniform uvec4 con0, con1, con2, con3;

AF4 FsrEasuRF(AF2 p) { return textureGather(uInputTexture, p, 0); }
AF4 FsrEasuGF(AF2 p) { return textureGather(uInputTexture, p, 1); }
AF4 FsrEasuBF(AF2 p) { return textureGather(uInputTexture, p, 2); }

out vec4 FragColor;

void main() {
    AF3 color;
    FsrEasuF(color, AU2(gl_FragCoord.xy), con0, con1, con2, con3);
    FragColor = vec4(color, 1.0);
}
