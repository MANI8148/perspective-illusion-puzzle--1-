#version 330 core
in  vec3  vColor;
in  float vLife;
out vec4  FragColor;

void main() {
    vec2  uv = gl_PointCoord - vec2(0.5);
    float r  = length(uv);
    if(r > 0.5) discard;

    // Bright core, soft fade to transparent edge
    float core  = smoothstep(0.5, 0.05, r);
    float glow  = smoothstep(0.5, 0.30, r) * 0.6;
    float alpha = (core + glow) * vLife;

    // Blow out the centre toward white
    vec3 col = mix(vColor * 1.8, vec3(1.0), core * 0.55);

    FragColor = vec4(col, alpha);
}
