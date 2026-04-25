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
uniform int   season; // 0=summer 1=spring 2=rainy 3=autumn 4=winter
// matType:
//   0 = standard platform (grass-top / dirt-sides)
//   1 = goal platform     (gold block shimmer)
//   2 = pillar            (cobblestone)
//   3 = player figure     (rim lit)
//   4 = illusion glow     (enchantment shimmer)
//   5 = particle          (emissive)
//   6 = obstacle/lava     (glowing lava cracks)

// ─── Utility ──────────────────────────────────────────────────────────────────
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
    return mix(mix(mix(hash(i),             hash(i+vec3(1,0,0)), f.x),
                   mix(hash(i+vec3(0,1,0)), hash(i+vec3(1,1,0)), f.x), f.y),
               mix(mix(hash(i+vec3(0,0,1)), hash(i+vec3(1,0,1)), f.x),
                   mix(hash(i+vec3(0,1,1)), hash(i+vec3(1,1,1)), f.x), f.y), f.z);
}
float fbm(vec3 p) {
    float v = 0.0, a = 0.5;
    for(int i = 0; i < 3; i++) { v += a * noise3(p); p *= 2.1; a *= 0.5; }
    return v;
}

// ─── Minecraft-style directional lighting ─────────────────────────────────────
// 6 distinct face brightnesses like the game does.
vec3 mcLight(vec3 col, vec3 N) {
    float topB    =  1.00 * max(dot(N, vec3( 0, 1, 0)), 0.0);
    float botB    =  0.50 * max(dot(N, vec3( 0,-1, 0)), 0.0);
    float northB  =  0.80 * max(dot(N, vec3( 0, 0,-1)), 0.0);
    float southB  =  0.80 * max(dot(N, vec3( 0, 0, 1)), 0.0);
    float eastB   =  0.60 * max(dot(N, vec3( 1, 0, 0)), 0.0);
    float westB   =  0.60 * max(dot(N, vec3(-1, 0, 0)), 0.0);
    float bright  = topB + botB + northB + southB + eastB + westB;
    // ambient floor so no face is pitch-black
    return col * max(bright, 0.38);
}

// ─── Season grass tint ────────────────────────────────────────────────────────
vec3 seasonGrass(vec3 base) {
    if (season == 1) return mix(base, vec3(0.55,0.95,0.22), 0.35); // spring: lime
    if (season == 2) return mix(base, vec3(0.22,0.48,0.18), 0.40); // rainy: dark
    if (season == 3) return mix(base, vec3(0.70,0.42,0.10), 0.45); // autumn: orange-brown
    if (season == 4) return mix(base, vec3(0.88,0.92,0.98), 0.55); // winter: snowy white
    return base; // summer: default
}

void main() {
    // ─── Wireframe overlay ────────────────────────────────────────────────────
    if (isWireframe) {
        FragColor = vec4(0.05, 0.05, 0.05, 0.85);
        return;
    }

    vec3 N = normalize(Normal);
    float topF  = clamp(dot(N, vec3(0,1,0)), 0.0, 1.0);

    vec3  surfaceCol = objectColor;

    // ─── matType 5: Particle (pure emissive) ──────────────────────────────────
    if (matType == 5) {
        FragColor = vec4(objectColor * 2.5, 0.85);
        return;
    }

    // ─── matType 0: Minecraft grass-top / dirt-sides ──────────────────────────
    if (matType == 0) {
        vec3 grassColor = seasonGrass(vec3(0.38, 0.72, 0.22));
        vec3 dirtColor  = vec3(0.545, 0.388, 0.196);

        if (topF > 0.7) {
            // Flat grass top with subtle noise variation
            float n = noise3(vec3(FragPos.x * 4.0, 0.0, FragPos.z * 4.0)) * 0.12;
            surfaceCol = grassColor * (0.90 + n);
            // Edge brightening (block rim highlight)
            float edge = max(abs(LocalPos.x), abs(LocalPos.z));
            float rim  = smoothstep(0.44, 0.50, edge);
            surfaceCol += grassColor * rim * 0.5;
        } else {
            // Dirt sides with thin green strip at top
            float n = noise3(FragPos * vec3(3.0, 1.5, 3.0)) * 0.15;
            surfaceCol = dirtColor * (0.80 + n);
            // Grass-cap strip at top edge of side face
            float ylocal = LocalPos.y;
            float grassStrip = smoothstep(0.38, 0.50, ylocal);
            surfaceCol = mix(surfaceCol, grassColor * 0.85, grassStrip * 0.9);
        }
        surfaceCol = mcLight(surfaceCol, N);
    }

    // ─── matType 1: Goal platform (gold block) ────────────────────────────────
    else if (matType == 1) {
        vec3 goldBase = vec3(0.92, 0.76, 0.10);
        float shine = noise3(FragPos * 6.0 + time * 0.6) * 0.15;
        float pulse = 0.5 + 0.5 * sin(time * 2.2);
        surfaceCol = goldBase * (0.85 + shine);
        // Animated cross-hatch shimmer
        float cx = fract(FragPos.x * 2.0 + time * 0.4);
        float cz = fract(FragPos.z * 2.0 - time * 0.4);
        float grid = max(smoothstep(0.85,1.0,cx), smoothstep(0.85,1.0,cz));
        surfaceCol += vec3(1.0, 0.90, 0.40) * grid * pulse * 1.2;
        surfaceCol = mcLight(surfaceCol, N);
    }

    // ─── matType 2: Pillar (cobblestone) ──────────────────────────────────────
    else if (matType == 2) {
        // Chunky cell-noise for cobblestone look
        vec3 fp = FragPos * 2.5;
        vec3 ic = floor(fp);
        float cell = hash(ic) * 0.35;
        float seam = step(0.85, fract(fp.x)) + step(0.85, fract(fp.y)) + step(0.85, fract(fp.z));
        seam = clamp(seam, 0.0, 1.0);
        surfaceCol = objectColor * (0.70 + cell);
        surfaceCol = mix(surfaceCol, vec3(0.15), seam * 0.6);
        surfaceCol = mcLight(surfaceCol, N);
    }

    // ─── matType 3: Player figure (rim lit, blocky) ───────────────────────────
    else if (matType == 3) {
        vec3 V3 = normalize(viewPos - FragPos);
        float rim = 1.0 - max(dot(N, V3), 0.0);
        rim = pow(rim, 2.5) * 1.2;
        surfaceCol = objectColor * (0.85 + rim * 0.4);
        surfaceCol = mcLight(surfaceCol, N);
    }

    // ─── matType 4: Illusion-aligned glow (enchantment shimmer) ──────────────
    else if (matType == 4) {
        float pulse = 0.5 + 0.5 * sin(time * 3.5);
        // Enchantment table-style: purple/blue sheen
        float n2 = noise3(FragPos * 3.0 - time * 0.5);
        vec3 enchant = vec3(0.35 + 0.3*sin(n2*6.28+time),
                            0.20 + 0.2*sin(n2*6.28+time+2.1),
                            0.80 + 0.2*sin(n2*6.28-time));
        surfaceCol = mix(objectColor, enchant, 0.55 + 0.35*pulse);
        // Blocky sparkle on top
        if (topF > 0.5) {
            float bsp = hash(floor(FragPos * 4.0 + time * 0.5));
            float spark = step(0.97, bsp) * pulse * 2.0;
            surfaceCol += vec3(0.8, 0.9, 1.0) * spark;
        }
        surfaceCol = mcLight(surfaceCol, N);
    }

    // ─── matType 6: Obstacle / Lava block ────────────────────────────────────
    else if (matType == 6) {
        // Dark base with animated orange-red cracks
        float pulse = 0.5 + 0.5 * sin(time * 2.8);
        float n1 = fbm(FragPos * 3.0 - time * 0.4);
        float n2 = fbm(FragPos * 6.0 + time * 0.6);
        vec3 darkBase = vec3(0.12, 0.04, 0.01);
        vec3 lavaGlow = vec3(1.0, 0.38 + 0.12*pulse, 0.0);
        float crack = smoothstep(0.3, 0.55, n1) * (0.5 + 0.5 * n2);
        surfaceCol = mix(darkBase, lavaGlow * (0.9 + 0.4*pulse), crack);
        // Top face: brighter lava pool
        float pool = 0.0;
        if (topF > 0.6) {
            pool = fbm(vec3(FragPos.x * 2.0, FragPos.y, FragPos.z * 2.0) + vec3(0.0, time * 0.3, 0.0));
            surfaceCol = mix(surfaceCol, lavaGlow * 1.4, smoothstep(0.3, 0.7, pool) * 0.7);
        }
        // Always emissive, skip mcLight
        FragColor = vec4(clamp(surfaceCol, 0.0, 1.0), 1.0);
        return;
    }

    // ─── Glow overlay for illusion-aligned blocks ─────────────────────────────
    if (isGlow) {
        float pulse = 0.5 + 0.5 * sin(time * 4.5);
        surfaceCol += objectColor * pulse * 0.35;
    }

    // Simple gamma + tone (much lighter than before, more Minecraft-bright)
    surfaceCol = clamp(surfaceCol, 0.0, 1.0);
    surfaceCol = pow(surfaceCol, vec3(1.0 / 2.0)); // softer gamma

    FragColor = vec4(surfaceCol, 1.0);
}
