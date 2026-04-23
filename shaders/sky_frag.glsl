#version 330 core
in  vec3 TexCoords;
out vec4 FragColor;
uniform float time;

// ── Hash / Noise ──────────────────────────────────────────────────────────────
float hash(vec3 p) {
    p = fract(p * vec3(443.8975, 397.2973, 491.1871));
    p += dot(p.zxy, p.yxz + 19.19);
    return fract(p.x * p.y * p.z);
}
float hash2(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}
float noise3(vec3 p) {
    vec3 i = floor(p); vec3 f = fract(p);
    f = f*f*(3.0 - 2.0*f);
    return mix(
        mix(mix(hash(i),             hash(i+vec3(1,0,0)), f.x),
            mix(hash(i+vec3(0,1,0)), hash(i+vec3(1,1,0)), f.x), f.y),
        mix(mix(hash(i+vec3(0,0,1)), hash(i+vec3(1,0,1)), f.x),
            mix(hash(i+vec3(0,1,1)), hash(i+vec3(1,1,1)), f.x), f.y), f.z);
}
float fbm(vec3 p) {
    float v = 0.0, a = 0.5;
    for(int i = 0; i < 5; i++) { v += a * noise3(p); p *= 2.09; a *= 0.5; }
    return v;
}

// ── Stars ─────────────────────────────────────────────────────────────────────
float stars(vec3 dir, float scale, float threshold) {
    vec3 p = floor(dir * scale);
    float h = hash(p);
    if(h > threshold) {
        vec3 center = (p + 0.5) / scale;
        float d = length(dir - normalize(center));
        float bright = (h - threshold) / (1.0 - threshold);
        return bright * smoothstep(0.007, 0.0, d);
    }
    return 0.0;
}

void main() {
    vec3 dir = normalize(TexCoords);

    // ── Deep space base ───────────────────────────────────────────────────────
    vec3 col = vec3(0.01, 0.01, 0.04);

    // ── Nebula layers ─────────────────────────────────────────────────────────
    float n1 = fbm(dir * 2.2 + vec3(0.0, time * 0.004, 0.0));
    float n2 = fbm(dir * 3.8 + vec3(time * 0.003, 0.0, 0.5));
    float n3 = fbm(dir * 1.1 + vec3(0.7, 0.0, time * 0.002));

    col += vec3(0.10, 0.02, 0.22) * n1 * n1 * 2.2;   // purple cloud
    col += vec3(0.00, 0.05, 0.20) * n2 * n2 * 1.8;   // deep blue
    col += vec3(0.00, 0.14, 0.18) * n1 * n2 * 3.0;   // teal overlap
    col += vec3(0.18, 0.02, 0.08) * n3 * n3 * 1.4;   // faint magenta wisp

    // ── Milky Way band ────────────────────────────────────────────────────────
    float band  = exp(-abs(dir.y) * 3.5);
    float milky = fbm(vec3(dir.xz * 4.0, 0.5)) * band;
    col += vec3(0.06, 0.08, 0.16) * milky * 1.5;

    // ── Stars at three scales ─────────────────────────────────────────────────
    float s  = stars(dir, 160.0, 0.994)
             + stars(dir, 300.0, 0.996) * 0.65
             + stars(dir,  75.0, 0.990) * 1.60;
    float twinkle = sin(time * 2.8 + hash(floor(dir * 180.0)) * 6.28) * 0.25 + 0.75;
    // Color-shift stars slightly (hot = blue-white, cool = orange-yellow)
    float starTemp = hash(floor(dir * 160.0));
    vec3  starCol  = mix(vec3(1.0, 0.75, 0.45), vec3(0.80, 0.90, 1.0), starTemp);
    col += starCol * s * twinkle;

    // ── Horizon inner glow ────────────────────────────────────────────────────
    float horiz = pow(max(0.0, 1.0 - abs(dir.y) * 5.0), 3.0) * 0.12;
    col += vec3(0.04, 0.02, 0.15) * horiz;

    // ── Bottom (ground-facing sky) darken ─────────────────────────────────────
    col *= (0.3 + 0.7 * clamp(dir.y * 2.0 + 0.5, 0.0, 1.0));

    FragColor = vec4(col, 1.0);
}
