#version 330 core
in  vec3 FragPos;
in  vec3 Normal;
in  vec3 LocalPos;
out vec4 FragColor;

uniform vec3  objectColor;
uniform vec3  viewPos;
uniform bool  isGlow;
uniform bool  isWireframe;
uniform float time;
uniform int   matType; 

// ── Noise Helpers ─────────────────────────────────────────────────────────────
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

void main() {
    if (isWireframe) {
        FragColor = vec4(0.02, 0.03, 0.08, 1.0);
        return;
    }

    // ── Pure Emissive (Particles) ─────────────────────────────────────────────
    if (matType == 5) {
        FragColor = vec4(objectColor * 2.0, 1.0);
        return;
    }

    vec3 N = normalize(Normal);
    vec3 V = normalize(viewPos - FragPos);
    float topF = clamp(dot(N, vec3(0, 1, 0)), 0.0, 1.0);

    vec3 surfaceCol = objectColor;
    float emission = 0.0;

    if (matType == 0 || matType == 2) {
        if (topF > 0.8) {
            vec2 vor = voronoi(FragPos.xz * 1.5);
            float crack = 1.0 - smoothstep(0.0, 0.06, vor.x);
            surfaceCol = objectColor * (0.8 + vor.y * 0.2);
            surfaceCol = mix(surfaceCol, vec3(0.01), crack * 0.8);
            float edge = max(abs(LocalPos.x), abs(LocalPos.z));
            float rim = smoothstep(0.44, 0.5, edge);
            surfaceCol += objectColor * rim * 2.0;
            emission += rim * 0.8;
        } else {
            float f = fbm(vec3(FragPos.xz * 0.5, FragPos.y * 2.0));
            float band = sin(FragPos.y * 8.0 + f * 3.0) * 0.5 + 0.5;
            surfaceCol = objectColor * (0.6 + band * 0.4);
        }
    } else if (matType == 1) {
        float f = fbm(vec3(FragPos * 1.2 + time * 0.5));
        surfaceCol = mix(vec3(0.1, 0.8, 0.3), vec3(0.05, 0.4, 0.2), f);
        if (topF > 0.8) {
            vec2 c = fract(FragPos.xz) - 0.5;
            float r = length(c);
            float rune = smoothstep(0.03, 0.0, abs(r - 0.3)) + smoothstep(0.03, 0.0, abs(r - 0.15));
            rune *= (0.5 + 0.5 * sin(time * 3.0 + r * 10.0));
            surfaceCol += vec3(0.4, 1.0, 0.6) * rune * 2.0;
            emission += rune;
        }
    } else if (matType == 4) {
        float p = 0.5 + 0.5 * sin(time * 2.0);
        float n = fbm(FragPos * 2.0 - time * 0.8);
        surfaceCol = mix(objectColor, vec3(1.0), n * 0.5);
        emission = 0.4 + 0.4 * n + 0.2 * p;
    }

    float diff = max(dot(N, normalize(vec3(0.5, 1.0, 0.3))), 0.0) * 0.6 + 0.4;
    vec3 result = surfaceCol * diff;
    if (isGlow) {
        float pulse = 0.5 + 0.5 * sin(time * 4.0);
        result += objectColor * pulse * 0.8;
        emission += pulse * 0.4;
    }
    result += surfaceCol * emission;
    result = result / (result + vec3(1.0));
    result = pow(result, vec3(1.0/2.2));
    FragColor = vec4(result, 1.0);
}
