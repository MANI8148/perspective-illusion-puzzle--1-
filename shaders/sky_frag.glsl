#version 330 core
out vec4 FragColor;
in vec3 TexCoords;
uniform float time;
uniform int   levelIndex; // 0-11, controls nebula palette

float hash(vec3 p) {
    p = fract(p * vec3(443.8975, 397.2973, 491.1871));
    p += dot(p.zxy, p.yxz + 19.19);
    return fract(p.x * p.y * p.z);
}
float noise(vec3 p) {
    vec3 i = floor(p); vec3 f = fract(p);
    f = f * f * (3.0 - 2.0 * f);
    return mix(mix(mix(hash(i),             hash(i+vec3(1,0,0)), f.x),
                   mix(hash(i+vec3(0,1,0)), hash(i+vec3(1,1,0)), f.x), f.y),
               mix(mix(hash(i+vec3(0,0,1)), hash(i+vec3(1,0,1)), f.x),
                   mix(hash(i+vec3(0,1,1)), hash(i+vec3(1,1,1)), f.x), f.y), f.z);
}
float fbm(vec3 p) {
    float v = 0.0, a = 0.5;
    for(int i = 0; i < 5; i++) { v += a * noise(p); p *= 2.1; a *= 0.5; }
    return v;
}

// Per-level nebula palette
vec3 nebulaColor1() {
    int l = levelIndex % 6;
    if(l == 0) return vec3(0.20, 0.05, 0.45);  // Purple
    if(l == 1) return vec3(0.05, 0.20, 0.45);  // Deep Blue
    if(l == 2) return vec3(0.00, 0.40, 0.30);  // Teal
    if(l == 3) return vec3(0.45, 0.10, 0.20);  // Crimson
    if(l == 4) return vec3(0.10, 0.35, 0.15);  // Emerald
    return vec3(0.30, 0.15, 0.00);              // Amber
}
vec3 nebulaColor2() {
    int l = (levelIndex / 2) % 4;
    if(l == 0) return vec3(0.05, 0.15, 0.35);  // Indigo
    if(l == 1) return vec3(0.00, 0.20, 0.30);  // Teal-Blue
    if(l == 2) return vec3(0.25, 0.05, 0.35);  // Violet
    return vec3(0.00, 0.30, 0.20);              // Sea Green
}

void main() {
    vec3 dir = normalize(TexCoords);

    // 1. Deep space base gradient
    float horiz = dot(dir, vec3(0, 1, 0));
    vec3 col = mix(vec3(0.01, 0.01, 0.03), vec3(0.02, 0.03, 0.10), clamp(horiz * 0.5 + 0.5, 0.0, 1.0));

    // 2. Procedural starfield — small + large stars
    float s1 = pow(hash(dir * 800.0), 50.0) * 2.5; // small sharp stars
    float s2 = pow(hash(dir * 300.0), 30.0) * 1.0; // larger softer stars
    // Twinkle
    s1 *= 0.6 + 0.4 * sin(time * 3.0 + hash(dir * 120.0) * 12.0);
    s2 *= 0.7 + 0.3 * sin(time * 1.5 + hash(dir * 60.0) * 8.0);
    // Color variation on bigger stars
    vec3 starColor = mix(vec3(0.9, 0.95, 1.0), vec3(1.0, 0.85, 0.7), hash(dir * 200.0));
    col += (s1 + s2 * 0.5) * starColor;

    // 3. Aurora bands
    float aurora = fbm(dir * 2.0 + time * 0.06) * fbm(dir * 4.0 - time * 0.04);
    aurora = pow(max(aurora - 0.1, 0.0), 1.5) * 3.0;
    // Aurora only near horizon
    float auroraFade = smoothstep(-0.1, 0.4, horiz) * smoothstep(0.8, 0.2, horiz);
    float auroraWave = sin(dir.x * 8.0 + time * 0.8) * 0.5 + 0.5;
    vec3 auroraCol = mix(nebulaColor1() * 2.5, nebulaColor2() * 2.5, auroraWave);
    col += auroraCol * aurora * auroraFade * 0.6;

    // 4. Nebula clouds
    float n1 = fbm(dir * 2.5 + time * 0.05);
    float n2 = fbm(dir * 1.8 - time * 0.03);
    col += nebulaColor1() * pow(n1, 3.5) * 1.2;
    col += nebulaColor2() * pow(n2, 3.0) * 0.8;

    // 5. Galaxy smear
    float smear = pow(fbm(dir * 1.2 + vec3(5.0, 3.0, 1.0)), 5.0) * 0.5;
    col += mix(nebulaColor1(), nebulaColor2(), 0.5) * smear;

    // 6. Horizon atmospheric glow
    float atmos = pow(1.0 - abs(horiz), 6.0);
    col += nebulaColor1() * atmos * 0.15;

    FragColor = vec4(col, 1.0);
}
