#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aUV;

out vec2 vUV;
out vec2 vPos;   // local [0..1] position for shape SDFs

uniform mat4 uProj;
uniform vec4 uRect;  // x,y,w,h in screen pixels (for SDF)

void main() {
    vUV  = aUV;
    vPos = aUV;   // UV already in [0,1] local space
    gl_Position = uProj * vec4(aPos, 0.0, 1.0);
}
