#version 420 core
out vec2 vTexCoord;
void main() {
    vec2 uv = vec2(float((gl_VertexID == 1) ? 2 : 0),
                   float((gl_VertexID == 2) ? 2 : 0));
    gl_Position = vec4(uv * 2.0 - 1.0, 0.0, 1.0);
    vTexCoord = uv;
}