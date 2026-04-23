#version 330 core
out vec4 FragColor;
in vec3 TexCoords;
uniform float time;

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

void main() {
    vec3 dir = normalize(TexCoords);
    
    // 1. Deep Space Base
    vec3 col = vec3(0.02, 0.03, 0.08);
    
    // 2. Procedural Starfield
    float stars = pow(hash(dir * 500.0), 40.0) * 1.5;
    // Twinkle
    stars *= 0.7 + 0.3 * sin(time * 2.0 + hash(dir * 100.0) * 10.0);
    col += stars;
    
    // 3. Aurora / Nebula Bands
    float n1 = fbm(dir * 2.5 + time * 0.05);
    float n2 = fbm(dir * 1.8 - time * 0.03);
    
    vec3 nebula1 = vec3(0.2, 0.05, 0.4) * pow(n1, 3.0); // Purple
    vec3 nebula2 = vec3(0.05, 0.15, 0.3) * pow(n2, 2.5); // Deep Blue
    vec3 nebula3 = vec3(0.0, 0.2, 0.2) * pow(fbm(dir * 4.0 + n1), 4.0); // Teal shimmer
    
    col += nebula1 + nebula2 + nebula3;
    
    // 4. Galaxy Smear (layered fbm)
    float smear = pow(fbm(dir * 1.2 + vec3(5.0)), 5.0) * 0.4;
    col += vec3(0.4, 0.3, 0.5) * smear;

    // Atmospheric hazing towards horizon (Y=0)
    float horiz = 1.0 - abs(dir.y);
    col += vec3(0.05, 0.08, 0.15) * pow(horiz, 8.0);

    FragColor = vec4(col, 1.0);
}
