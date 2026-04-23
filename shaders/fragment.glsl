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
// matType:
//   0 = standard platform
//   1 = goal platform  (green runes)
//   2 = pillar         (stone bands)
//   3 = player figure  (rim lit)
//   4 = illusion glow  (iridescent pulse)
//   5 = particle       (pure emissive)

// ─── Utility ───────────────────────────────────────────────────────────────
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

// ─── Lighting ──────────────────────────────────────────────────────────────
vec3 blinnPhong(vec3 col, vec3 N, vec3 fragPos, float emissive) {
    vec3 L  = normalize(vec3(0.55, 1.0, 0.35));
    vec3 V  = normalize(viewPos - fragPos);
    vec3 H  = normalize(L + V);

    float diff  = max(dot(N, L), 0.0) * 0.65 + 0.35; // ambient + diffuse
    float spec  = pow(max(dot(N, H), 0.0), 64.0) * 0.5;
    vec3  result = col * diff + vec3(1.0) * spec * (0.3 + 0.7 * (1.0 - dot(N, vec3(0,1,0))));
    result += col * emissive;
    return result;
}

void main() {
    // ─── Wireframe overlay ────────────────────────────────────────────────
    if (isWireframe) {
        FragColor = vec4(0.02, 0.04, 0.12, 0.9);
        return;
    }

    // ─── Particle (pure emissive) ─────────────────────────────────────────
    if (matType == 5) {
        FragColor = vec4(objectColor * 2.5, 0.85);
        return;
    }

    vec3 N    = normalize(Normal);
    float topF = clamp(dot(N, vec3(0, 1, 0)), 0.0, 1.0);
    float sideF = 1.0 - topF;

    vec3  surfaceCol = objectColor;
    float emission   = 0.0;

    // ─── matType 0: Standard platform ─────────────────────────────────────
    if (matType == 0 || matType == 2) {
        if (topF > 0.7) {
            // Cracked-tile top face via Voronoi
            vec2 vor = voronoi(FragPos.xz * 2.0);
            float crack = 1.0 - smoothstep(0.0, 0.05, vor.x);
            surfaceCol = objectColor * (0.75 + vor.y * 0.3);
            surfaceCol = mix(surfaceCol, objectColor * 0.1, crack * 0.7);
            // Glowing edge rim
            float edge = max(abs(LocalPos.x), abs(LocalPos.z));
            float rim  = smoothstep(0.42, 0.5, edge);
            surfaceCol += objectColor * rim * 1.8;
            emission   += rim * 0.6;
        } else {
            // Side face: layered noise bands
            float n = fbm(vec3(FragPos.xz * 0.4, FragPos.y * 1.5));
            float band = sin(FragPos.y * 10.0 + n * 4.0) * 0.5 + 0.5;
            surfaceCol = objectColor * (0.55 + band * 0.45);
            // Vertical edge glow
            float vedge = max(abs(LocalPos.x), abs(LocalPos.z));
            float vrim  = smoothstep(0.45, 0.5, vedge);
            surfaceCol += objectColor * vrim * 1.2;
            emission   += vrim * 0.3;
        }
        if (matType == 2) {
            // Pillar: stone banding override
            float band2 = sin(FragPos.y * 6.0 + noise3(FragPos * 1.2) * 2.0) * 0.5 + 0.5;
            surfaceCol *= (0.6 + band2 * 0.4);
        }
    }
    // ─── matType 1: Goal platform (animated rune ring) ────────────────────
    else if (matType == 1) {
        float n = fbm(FragPos * 1.0 + time * 0.4);
        surfaceCol = mix(vec3(0.08, 0.72, 0.28), vec3(0.02, 0.38, 0.15), n);
        if (topF > 0.7) {
            // Rotating rune rings
            vec2 c  = fract(FragPos.xz) - 0.5;
            float r = length(c);
            float theta = atan(c.y, c.x);
            float ring1 = smoothstep(0.03, 0.0, abs(r - 0.36)) * (0.6 + 0.4 * sin(theta * 6.0 - time * 2.5));
            float ring2 = smoothstep(0.025, 0.0, abs(r - 0.22)) * (0.6 + 0.4 * sin(theta * 4.0 + time * 3.0));
            float ring3 = smoothstep(0.02, 0.0, abs(r - 0.10)) * (0.6 + 0.4 * sin(time * 6.0));
            float runes = ring1 + ring2 + ring3;
            runes *= (0.5 + 0.5 * sin(time * 2.0));
            surfaceCol += vec3(0.3, 1.0, 0.5) * runes * 2.5;
            emission   += runes * 1.2;
        }
    }
    // ─── matType 3: Player figure (rim light) ─────────────────────────────
    else if (matType == 3) {
        vec3 V3 = normalize(viewPos - FragPos);
        float rim = 1.0 - max(dot(N, V3), 0.0);
        rim = pow(rim, 3.0) * 1.5;
        surfaceCol = objectColor;
        surfaceCol += vec3(1.0, 0.8, 0.6) * rim * 0.8;
        emission = rim * 0.4;
    }
    // ─── matType 4: Illusion-aligned glow (iridescent) ────────────────────
    else if (matType == 4) {
        float pulse = 0.5 + 0.5 * sin(time * 3.5);
        float n2    = fbm(FragPos * 2.5 - time * 0.6);
        // Iridescent hue shift
        float hueShift = n2 * 2.0 + time * 0.4;
        vec3 iri = vec3(
            0.5 + 0.5 * sin(hueShift),
            0.5 + 0.5 * sin(hueShift + 2.094),
            0.5 + 0.5 * sin(hueShift + 4.188)
        );
        surfaceCol = mix(objectColor, iri, 0.5 + 0.4 * pulse);

        // Sparkling top face
        if (topF > 0.5) {
            float spark = pow(hash2(fract(FragPos.xz * 8.0 + time * 0.3)), 8.0) * 4.0;
            surfaceCol += vec3(1.0, 0.95, 0.7) * spark;
            emission += spark * 0.5;
        }

        // Voronoi crack lines glowing gold
        vec2 vor2 = voronoi(FragPos.xz * 2.5);
        float vEdge = 1.0 - smoothstep(0.0, 0.05, vor2.x);
        surfaceCol += vec3(1.0, 0.8, 0.2) * vEdge * pulse * 1.5;
        emission += vEdge * pulse * 0.8;
    }

    // ─── Final lighting ───────────────────────────────────────────────────
    vec3 result = blinnPhong(surfaceCol, N, FragPos, emission);

    // Glow pulsing overlay (for illusion-aligned platforms)
    if (isGlow) {
        float pulse = 0.5 + 0.5 * sin(time * 4.5);
        result += objectColor * pulse * 0.6;
    }

    // Tone mapping (ACES filmic)
    result = result * (2.51 * result + 0.03) / (result * (2.43 * result + 0.59) + 0.14);
    result = clamp(result, 0.0, 1.0);
    // Gamma correction
    result = pow(result, vec3(1.0 / 2.2));

    FragColor = vec4(result, 1.0);
}
