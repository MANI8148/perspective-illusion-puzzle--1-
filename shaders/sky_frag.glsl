#version 330 core
out vec4 FragColor;
in  vec3 TexCoords;

uniform float time;
uniform int   levelIndex;
uniform int   season; // 0=summer 1=spring 2=rainy 3=autumn 4=winter

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
    for(int i = 0; i < 4; i++) { v += a * noise(p); p *= 2.0; a *= 0.5; }
    return v;
}

void main() {
    vec3 dir    = normalize(TexCoords);
    float horiz = dot(dir, vec3(0, 1, 0));   // -1=below, 0=horizon, 1=zenith
    float above = clamp(horiz, 0.0, 1.0);    // 0..1 above horizon

    vec3 col = vec3(0.0);

    // ── Season sky gradients ──────────────────────────────────────────────────
    if (season == 0) {
        // Summer: bright blue sky
        vec3 zenith  = vec3(0.20, 0.48, 0.90);
        vec3 horizon = vec3(0.62, 0.82, 1.00);
        col = mix(horizon, zenith, above);
    }
    else if (season == 1) {
        // Spring: soft sky with slight pink blush
        vec3 zenith  = vec3(0.38, 0.62, 0.95);
        vec3 horizon = vec3(0.82, 0.88, 1.00);
        vec3 blush   = vec3(1.00, 0.78, 0.78);
        col = mix(horizon, zenith, above);
        col = mix(col, blush, smoothstep(0.3,0.0,above) * 0.35);
    }
    else if (season == 2) {
        // Rainy: overcast gray
        vec3 zenith  = vec3(0.28, 0.32, 0.35);
        vec3 horizon = vec3(0.46, 0.50, 0.52);
        col = mix(horizon, zenith, above);
        // Heavy cloud layer
        float cloud = fbm(dir * 3.0 + time * 0.04);
        col = mix(col, vec3(0.50, 0.52, 0.55), smoothstep(0.2, 0.7, cloud) * 0.6);
        // Rain streaks (fast angled lines on upper hemisphere)
        if (horiz > 0.0) {
            float ang = dir.x * 80.0 + dir.z * 30.0 + time * 6.0;
            float streak = step(0.98, fract(ang * 0.15));
            col = mix(col, vec3(0.55, 0.65, 0.78), streak * above * 0.5);
        }
    }
    else if (season == 3) {
        // Autumn: warm amber horizon, deep blue top
        vec3 zenith  = vec3(0.16, 0.30, 0.65);
        vec3 horizon = vec3(0.92, 0.62, 0.22);
        vec3 midday  = vec3(0.55, 0.70, 0.92);
        col = mix(horizon, midday, smoothstep(0.0, 0.3, above));
        col = mix(col, zenith, smoothstep(0.3, 1.0, above));
    }
    else {
        // Winter: pale cold sky, slight gray-blue
        vec3 zenith  = vec3(0.50, 0.62, 0.82);
        vec3 horizon = vec3(0.78, 0.84, 0.95);
        col = mix(horizon, zenith, above);
        // Light overcast wisp
        float wc = fbm(dir * 2.5 + time * 0.02);
        col = mix(col, vec3(0.90, 0.92, 0.96), smoothstep(0.5, 0.8, wc) * 0.35);
    }

    // ── Ground-below: dark base ───────────────────────────────────────────────
    if (horiz < 0.0) {
        float t2 = clamp(-horiz * 3.0, 0.0, 1.0);
        col = mix(col, vec3(0.10, 0.08, 0.06), t2);
    }

    // ── Clouds (all seasons except rainy handled above) ───────────────────────
    if (season != 2 && horiz > 0.05) {
        float cloud = fbm(dir * 1.8 + time * 0.025);
        float cloudMask = smoothstep(0.48, 0.62, cloud);
        // Cloud color varies by season
        vec3 cloudCol;
        if      (season == 0) cloudCol = vec3(0.98, 0.98, 1.00);
        else if (season == 1) cloudCol = vec3(1.00, 0.94, 0.96);
        else if (season == 3) cloudCol = vec3(0.95, 0.82, 0.62);
        else                  cloudCol = vec3(0.88, 0.90, 0.96);
        // Clouds only in the middle band, not at zenith
        float band = smoothstep(0.0, 0.3, horiz) * smoothstep(0.9, 0.4, horiz);
        col = mix(col, cloudCol, cloudMask * band * 0.85);
    }

    // ── Sun disk (visible in summer / spring / autumn) ────────────────────────
    if (season != 2 && season != 4) {
        vec3 sunDir = normalize(vec3(0.5, 0.6, -0.5));
        float sunDot = dot(dir, sunDir);
        float sunDisk   = smoothstep(0.9990, 0.9999, sunDot);
        float sunCorona = smoothstep(0.990,  0.9990, sunDot) * 0.4;
        vec3 sunCol;
        if (season == 3) sunCol = vec3(1.0, 0.72, 0.30); // autumn orange sun
        else             sunCol = vec3(1.0, 0.98, 0.85);
        col += sunCol * (sunDisk + sunCorona);
    }

    // ── Snow specks (winter) ───────────────────────────────────────────────────
    if (season == 4 && horiz > 0.0) {
        float sp = pow(hash(dir * 600.0 + time * 0.2), 30.0);
        col += vec3(0.95, 0.97, 1.00) * sp * above * 1.5;
    }

    // ── Stars at night-like top for autumn/winter ─────────────────────────────
    if ((season == 3 || season == 4) && horiz > 0.4) {
        float star = pow(hash(dir * 500.0), 45.0) * 1.2;
        col += vec3(0.95, 0.95, 1.00) * star * (above - 0.4) * 2.0;
    }

    FragColor = vec4(clamp(col, 0.0, 1.0), 1.0);
}
