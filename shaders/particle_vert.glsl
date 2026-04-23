#version 330 core
layout(location = 0) in vec3  aPos;
layout(location = 1) in vec3  aColor;
layout(location = 2) in float aSize;
layout(location = 3) in float aLife;   // normalized 0..1

out vec3  vColor;
out float vLife;

uniform mat4  VP;
uniform vec3  viewPos;

void main() {
    vColor      = aColor;
    vLife       = aLife;
    gl_Position = VP * vec4(aPos, 1.0);
    float dist  = length(viewPos - aPos);
    gl_PointSize = aSize * 280.0 / max(dist, 0.5);
}
