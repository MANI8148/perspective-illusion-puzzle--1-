#version 330 core
in  vec2 vUV;
in  vec2 vPos;
out vec4 FragColor;

uniform vec4  uColor;       // base color (RGBA)
uniform vec4  uColor2;      // gradient end color
uniform float uRadius;      // corner radius in UV space (0=sharp)
uniform float uTime;
uniform int   uMode;
// Mode 0 = solid rect
// Mode 1 = gradient (top->bottom)
// Mode 2 = glass panel
// Mode 3 = button hover glow
// Mode 4 = level icon bg
// Mode 5 = progress bar

// Rounded rect SDF in UV space
float roundedBox(vec2 uv, float r) {
    vec2 d = abs(uv - 0.5) - (0.5 - r);
    return length(max(d, 0.0)) - r;
}

// Noise for glass effect
float hash2(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

void main() {
    float rr  = uRadius;
    float sdf = roundedBox(vPos, rr);
    float aa  = 0.004;

    if (uMode == 0) {
        // Solid with rounded corners
        float alpha = smoothstep(aa, -aa, sdf) * uColor.a;
        FragColor = vec4(uColor.rgb, alpha);

    } else if (uMode == 1) {
        // Gradient top->bottom
        float t   = vPos.y;
        vec3  col = mix(uColor.rgb, uColor2.rgb, t);
        float alpha = smoothstep(aa, -aa, sdf) * uColor.a;
        FragColor = vec4(col, alpha);

    } else if (uMode == 2) {
        // Glassmorphism panel
        float alpha = smoothstep(aa, -aa, sdf);
        // Inner glass gradient
        vec3 glass  = mix(uColor.rgb * 1.3, uColor.rgb * 0.7, vPos.y);
        // Top edge highlight
        float edge  = smoothstep(0.0, 0.04, vPos.y) * (1.0 - smoothstep(0.04, 0.08, vPos.y));
        glass += vec3(0.4) * edge * 0.4;
        // Subtle noise grain
        float grain = (hash2(vPos * 400.0) - 0.5) * 0.04;
        glass += grain;
        // Border glow
        float border = smoothstep(-aa*2.0, 0.0, -sdf) * smoothstep(0.012, 0.004, -sdf);
        glass += uColor2.rgb * border * 0.7;
        FragColor = vec4(glass, alpha * uColor.a);

    } else if (uMode == 3) {
        // Button with hover pulse
        float pulse = 0.5 + 0.5 * sin(uTime * 3.0);
        float alpha = smoothstep(aa, -aa, sdf) * uColor.a;
        vec3  col   = mix(uColor.rgb, uColor2.rgb, vPos.y * 0.5 + 0.25);
        // Inner glow
        float glow  = smoothstep(0.5, 0.0, abs(sdf + 0.01)) * pulse * 0.4;
        col += uColor2.rgb * glow;
        // Top sheen
        float sheen = (1.0 - vPos.y) * smoothstep(0.0, 0.3, vPos.y) * 0.25;
        col += sheen;
        // Border
        float bord  = smoothstep(-aa, 0.0, -sdf) * smoothstep(0.008, 0.002, -sdf);
        col += uColor2.rgb * bord;
        FragColor = vec4(col, alpha);

    } else if (uMode == 4) {
        // Level icon background with animated hex pattern
        float alpha = smoothstep(aa, -aa, sdf) * uColor.a;
        vec2  p2 = vPos * 6.0;
        float hex = abs(sin(p2.x * 1.732 + uTime * 0.5) * sin(p2.y + uTime * 0.3));
        vec3  col = mix(uColor.rgb * 0.6, uColor.rgb * 1.2, hex * 0.3 + 0.7 * vPos.y);
        // Center sparkle
        float dist = length(vPos - 0.5);
        float spark = smoothstep(0.3, 0.0, dist) * (0.5 + 0.5 * sin(uTime * 4.0));
        col += uColor2.rgb * spark * 0.5;
        FragColor = vec4(col, alpha);

    } else if (uMode == 5) {
        // Progress bar fill
        float alpha = smoothstep(aa, -aa, sdf) * uColor.a;
        vec3 col = mix(uColor.rgb, uColor2.rgb, vPos.x);
        float shine = sin(vPos.x * 3.14159) * 0.3;
        col += shine;
        FragColor = vec4(col, alpha);

    } else {
        FragColor = uColor;
    }
}
