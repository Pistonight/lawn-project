#version 420 core
#define A_GPU 1
#define A_GLSL 1
#TXTPP#include ../../lawn-libs/FidelityFX-FSR/include/fsr/ffx_a.h
#define FSR_RCAS_F 1
#TXTPP#include ../../lawn-libs/FidelityFX-FSR/include/fsr/ffx_fsr1.h

uniform sampler2D uInputTexture;
uniform uvec4 con;

AF4 FsrRcasLoadF(ASU2 p) { return texelFetch(uInputTexture, p, 0); }
void FsrRcasInputF(inout AF1 r, inout AF1 g, inout AF1 b) {}

out vec4 FragColor;

void main() {
    AF1 r, g, b;
    FsrRcasF(r, g, b, AU2(gl_FragCoord.xy), con);
    FragColor = vec4(r, g, b, 1.0);
}
