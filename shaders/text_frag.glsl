#version 330 core
in  vec2 vUV;
out vec4 FragColor;

uniform sampler2D uFontTex;
uniform vec4      uColor;   // text color (rgba)
uniform float     uGlow;    // 0=none, 1=full glow

void main() {
    float alpha = texture(uFontTex, vUV).r;
    if (uGlow > 0.0) {
        // Soft glow halo
        float glow  = smoothstep(0.3, 0.6, alpha) * uGlow * 0.5;
        vec3  glowC = uColor.rgb * 1.8;
        vec3  col   = mix(glowC * glow, uColor.rgb, smoothstep(0.4, 0.6, alpha));
        FragColor   = vec4(col, alpha * uColor.a);
    } else {
        float a = smoothstep(0.35, 0.55, alpha);
        FragColor = vec4(uColor.rgb, a * uColor.a);
    }
}
