#version 330 core

in  vec3 FragPos;
in  vec3 Normal;
in  vec3 LocalPos;
in  vec2 TexCoord;
out vec4 FragColor;

uniform vec3  objectColor;
uniform vec3  viewPos;
uniform bool  isGlow;
uniform bool  isWireframe;
uniform float time;
uniform int   matType;
// matType: 0=normal platform, 1=goal, 2=start, 3=player/orb, 4=edge strip, 5=beam/emissive

// ── Noise Library ─────────────────────────────────────────────────────────────
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
    f = f * f * (3.0 - 2.0 * f);
    return mix(
        mix(mix(hash(i),             hash(i+vec3(1,0,0)), f.x),
            mix(hash(i+vec3(0,1,0)), hash(i+vec3(1,1,0)), f.x), f.y),
        mix(mix(hash(i+vec3(0,0,1)), hash(i+vec3(1,0,1)), f.x),
            mix(hash(i+vec3(0,1,1)), hash(i+vec3(1,1,1)), f.x), f.y), f.z);
}
float fbm(vec3 p) {
    float v = 0.0, a = 0.5;
    for(int i = 0; i < 4; i++) { v += a * noise3(p); p *= 2.1; a *= 0.5; }
    return v;
}
vec2 voronoi(vec2 p) {
    vec2 ip = floor(p), fp = fract(p);
    float md = 1e9; vec2 mp = vec2(0.0);
    for(int j = -1; j <= 1; j++) for(int i = -1; i <= 1; i++) {
        vec2 b = vec2(i, j);
        vec2 r = b + vec2(hash2(ip+b), hash2(ip+b+vec2(3.7,1.3))) - fp;
        float d = length(r);
        if(d < md) { md = d; mp = ip + b; }
    }
    return vec2(md, hash2(mp));
}

void main() {

    // ── Wireframe / edge outline pass ─────────────────────────────────────────
    if (isWireframe) {
        FragColor = vec4(0.02, 0.03, 0.10, 1.0);
        return;
    }

    // ── Emissive-only materials (edge strips, beams) ──────────────────────────
    if (matType == 4 || matType == 5) {
        float p   = 0.5 + 0.5 * sin(time * 4.0);
        vec3  col = objectColor * (1.2 + 0.8 * p);
        col       = col / (col + vec3(1.0));
        col       = pow(col, vec3(1.0 / 2.2));
        FragColor = vec4(col, 1.0);
        return;
    }

    vec3 N = normalize(Normal);
    vec3 V = normalize(viewPos - FragPos);

    // ── Face orientation ──────────────────────────────────────────────────────
    float topF  = clamp(dot(N, vec3(0, 1, 0)),  0.0, 1.0);
    float botF  = clamp(dot(N, vec3(0,-1, 0)), 0.0, 1.0);

    // World-space UV for patterns
    vec2 topUV  = FragPos.xz * 1.8;
    vec2 sideUV = abs(N.x) > abs(N.z) ? FragPos.zy * 1.4 : FragPos.xy * 1.4;

    // ── Per-material surface color ────────────────────────────────────────────
    vec3 surfaceCol = objectColor;
    float emission  = 0.0;

    // ─────────────────────────────────────────────────────────────────────────
    if (matType == 0 || matType == 2) {
    // NORMAL / START PLATFORM — Ancient stone with energy veins
    // ─────────────────────────────────────────────────────────────────────────
        if (topF > 0.5) {
            // Voronoi cell cracks
            vec2 vor   = voronoi(topUV * 1.6);
            float crack = 1.0 - smoothstep(0.0, 0.07, vor.x);
            surfaceCol  = objectColor * (0.82 + vor.y * 0.18);
            // Dark crack lines
            surfaceCol  = mix(surfaceCol, vec3(0.01, 0.02, 0.06), crack * 0.75);
            // Glowing energy inside cracks
            float vein  = smoothstep(0.06, 0.0, vor.x) * 0.40;
            surfaceCol += objectColor * vein;
            emission   += vein * 0.15;
            // FBM roughness variation
            surfaceCol *= (0.88 + fbm(vec3(topUV * 0.9, 0.0)) * 0.24);
        } else {
            // Side faces: layered horizontal stone bands
            float fbmV  = fbm(vec3(sideUV * 0.8, 0.2));
            float band   = sin(sideUV.y * 6.0 + fbmV * 2.5) * 0.5 + 0.5;
            surfaceCol   = objectColor * (0.72 + band * 0.28);
            surfaceCol  *= (0.88 + noise3(vec3(sideUV, 0.5)) * 0.20);
            // Horizontal seam lines
            float seam   = smoothstep(0.02, 0.0, abs(fract(sideUV.y * 1.5) - 0.5) - 0.45);
            surfaceCol   = mix(surfaceCol, vec3(0.02, 0.02, 0.05), seam * 0.5);
        }

    // ─────────────────────────────────────────────────────────────────────────
    } else if (matType == 1) {
    // GOAL PLATFORM — Emerald crystal with animated runes
    // ─────────────────────────────────────────────────────────────────────────
        float fbmV   = fbm(vec3(topUV * 1.2, time * 0.25));
        surfaceCol   = mix(objectColor * 0.7, objectColor * 1.4, fbmV);

        if (topF > 0.5) {
            // Animated concentric runic rings
            vec2  c     = fract(FragPos.xz) - 0.5;
            float r1    = abs(length(c) - 0.28);
            float r2    = abs(length(c) - 0.46);
            float rune  = smoothstep(0.04, 0.0, r1) + smoothstep(0.04, 0.0, r2);
            float anim  = sin(time * 2.5 + length(c) * 8.0) * 0.5 + 0.5;
            rune       *= anim;
            surfaceCol += objectColor * rune * 2.2;
            emission   += rune * 0.9;

            // Crystal facet voronoi
            vec2 vor    = voronoi(topUV * 2.2);
            surfaceCol *= (0.80 + vor.y * 0.40);
            float sparkle = pow(vor.y, 8.0) * 0.6 * sin(time * 5.0 + vor.y * 10.0);
            surfaceCol += vec3(0.5, 1.0, 0.6) * sparkle;
        } else {
            // Glassy side: subtle vertical bands + inner glow
            float glow  = fbm(vec3(sideUV * 0.6, time * 0.3)) * 0.5;
            surfaceCol += objectColor * glow;
            emission   += glow * 0.3;
        }

    // ─────────────────────────────────────────────────────────────────────────
    } else if (matType == 3) {
    // PLAYER — Glowing energy orb
    // ─────────────────────────────────────────────────────────────────────────
        float fbmV   = fbm(N * 2.5 + vec3(time * 0.6, 0.0, time * 0.4));
        float pulse  = sin(time * 4.5) * 0.5 + 0.5;
        surfaceCol   = mix(objectColor, objectColor * 2.2, fbmV);
        // Energy swirl
        float swirl  = sin(N.x * 7.0 + N.y * 5.0 - time * 3.5) * 0.5 + 0.5;
        swirl       += sin(N.z * 5.0 - N.y * 6.0 + time * 2.8) * 0.3;
        surfaceCol  += objectColor * swirl * 0.7;
        // Bright core at top of sphere
        float core   = pow(clamp(N.y, 0.0, 1.0), 3.0) * (0.6 + 0.4 * pulse);
        surfaceCol  += vec3(1.0) * core * 0.5;
        emission     = 0.5 + pulse * 0.5 + fbmV * 0.3;
    }

    // ── 3-Point Lighting ──────────────────────────────────────────────────────
    vec3 faceCol = surfaceCol * (1.0 + 0.22 * topF - 0.24 * botF);

    // Key — warm directional sun
    vec3  kDir = normalize(vec3(4.0, 10.0, 5.0));
    float dk   = max(dot(N, kDir), 0.0);
    vec3  Hk   = normalize(kDir + V);
    float sk   = pow(max(dot(N, Hk), 0.0), 128.0);
    vec3  key  = faceCol * dk * vec3(1.0, 0.95, 0.85)
               + sk * 0.55 * vec3(1.0, 0.98, 0.90);

    // Fill — cool sky bounce
    vec3  fDir = normalize(vec3(-5.0, 6.0, -8.0));
    float df   = max(dot(N, fDir), 0.0) * 0.35;
    vec3  fill = faceCol * df * vec3(0.45, 0.60, 1.0);

    // Back — camera-relative rim so no face is ever fully black
    vec3  bDir = normalize(-V + vec3(0.0, 0.5, 0.0));
    float db   = max(dot(N, bDir), 0.0) * 0.20;
    vec3  back = faceCol * db * vec3(0.55, 0.65, 1.0);

    // Hemisphere ambient
    float hemi = N.y * 0.5 + 0.5;
    vec3  amb  = mix(vec3(0.08, 0.07, 0.14), vec3(0.22, 0.28, 0.52), hemi) * faceCol;

    // Fresnel rim
    float rim    = pow(1.0 - clamp(dot(V, N), 0.0, 1.0), 3.5) * 0.55;
    vec3  rimCol = rim * vec3(0.38, 0.52, 1.0);

    // Top AO bevel
    float cx  = 1.0 - abs(LocalPos.x * 2.0);
    float cz  = 1.0 - abs(LocalPos.z * 2.0);
    float ao  = 0.65 + 0.35 * cx * cz;
    vec3  topHL = vec3(topF * ao * 0.22);

    vec3 result = amb + key + fill + back + rimCol + topHL;

    // Add self-emission
    result += surfaceCol * emission;

    // ── Glow pulse (illusion-aligned / goal platforms) ────────────────────────
    if (isGlow) {
        float p  = 0.5 + 0.5 * sin(time * 3.5);
        result  += objectColor * 0.55 * p;
        result  += topHL * 0.5 * p;
        result  += rimCol * 0.8 * p;
        result  += topF * 0.30 * p * vec3(1.0, 0.88, 0.30);
    }

    // ── Tone-map (Reinhard) + gamma ──────────────────────────────────────────
    result = result / (result + vec3(1.0));
    result = pow(result, vec3(1.0 / 2.2));

    FragColor = vec4(result, 1.0);
}
