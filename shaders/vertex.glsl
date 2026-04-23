#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;
out vec3 LocalPos;
out vec2 TexCoord;

uniform mat4 MVP;
uniform mat4 model;

void main() {
    LocalPos = aPos;
    FragPos  = vec3(model * vec4(aPos, 1.0));
    Normal   = normalize(mat3(transpose(inverse(model))) * aNormal);

    // Tri-planar UV: project onto dominant axis
    vec3 n = abs(aNormal);
    if      (n.y >= n.x && n.y >= n.z) TexCoord = aPos.xz + 0.5;
    else if (n.x >= n.y && n.x >= n.z) TexCoord = aPos.zy + 0.5;
    else                                TexCoord = aPos.xy + 0.5;

    gl_Position = MVP * vec4(aPos, 1.0);
}
