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
    for(int i = 0; i < 5; i++) { v += a * noise(p); p *= 2.1; a *= 0.5; }
    return v;
}
float fbm2(vec3 p) {
    float v = 0.0, a = 0.5;
    for(int i = 0; i < 6; i++) { v += a * noise(p); p *= 2.0; a *= 0.5; }
    return v;
}
float starField(vec3 dir, float density, float seed) {
    return pow(hash(floor(dir * density + seed)), 38.0);
}
float diskGlow(float d, float core, float corona) {
    return smoothstep(core, core*0.5, d) + smoothstep(corona, core, d) * 0.45;
}
float aurora(vec3 dir, float band, float speed, float seed) {
    float lat  = dir.y - band;
    float wave = sin(dir.x * 4.0 + seed + time * speed) * 0.12
               + sin(dir.z * 3.0 - seed + time * speed * 0.7) * 0.08;
    float mask = exp(-abs(lat - wave) * 20.0);
    return mask * smoothstep(0.0, 0.3, dir.y) * smoothstep(0.9, 0.4, dir.y);
}

vec3 levelScenery(vec3 dir, float horiz, float above, float now) {
    vec3 s = vec3(0.0);

    if (levelIndex == 0) {
        // Summer: crepuscular sun rays + mountain silhouettes
        vec3 sunDir = normalize(vec3(0.55, 0.60, -0.50));
        float ray = 0.0;
        for(int k = 0; k < 5; k++) {
            float a = float(k) * 1.2566;
            vec3 side = vec3(cos(a), sin(a)*0.3, sin(a));
            ray += smoothstep(0.985, 0.999, dot(dir, normalize(sunDir + side*0.04))) * 0.18;
        }
        s += vec3(1.0, 0.90, 0.60) * ray * above;
        float mtn = sin(dir.x * 6.0 + dir.z * 4.0) * 0.5 + 0.5;
        mtn = smoothstep(0.42, 0.50, mtn) * smoothstep(0.12, 0.0, horiz + 0.10);
        s += vec3(0.15, 0.35, 0.60) * mtn * 0.45;
    }
    else if (levelIndex == 1) {
        // Spring: drifting cherry blossom petals + rainbow arc
        float petal = 0.0;
        for(int k = 0; k < 6; k++) {
            float f = float(k);
            vec3 pd = dir + vec3(f*0.31, f*0.17, f*0.27);
            float ang = pd.x * 50.0 + pd.z * 30.0 + now * (0.4 + f*0.07) + f*3.14;
            petal += step(0.97, fract(ang * 0.07))
                   * smoothstep(0.12, 0.35, horiz) * smoothstep(0.85, 0.30, horiz);
        }
        s += vec3(1.0, 0.70, 0.80) * petal * 0.9;
        s += vec3(1.0, 0.72, 0.80) * smoothstep(0.25, 0.0, horiz) * 0.5;
        if (horiz > 0.0) {
            float rd = dot(dir, normalize(vec3(-0.4, 0.3, 0.8)));
            float rb = exp(-(abs(rd - 0.63)) * (abs(rd - 0.63)) * 200.0);
            float hue = (rd - 0.55) * 10.0;
            vec3 rcol = 0.5 + 0.5*vec3(cos(hue*6.28), cos(hue*6.28+2.09), cos(hue*6.28+4.19));
            s += rcol * rb * 0.35 * above;
        }
    }
    else if (levelIndex == 2) {
        // Rainy: random lightning bolt flashes + dark anvil clouds
        float lseed = hash(vec3(floor(now * 0.25) * 4.0, 1.7, 3.3));
        float ltrig = hash(vec3(floor(now * 0.25) * 4.0 + 1.0, 2.1, 4.5));
        float lflash = 0.0;
        if (ltrig > 0.55) {
            float lp = fract(now * 0.25) * 4.0;
            lflash = exp(-lp * 8.0) * 0.7 + exp(-(lp-0.25)*(lp-0.25)*60.0)*0.5;
        }
        float bx = lseed * 2.0 - 1.0;
        float bolt = exp(-(dir.x-bx)*(dir.x-bx)*80.0) * exp(-dir.z*dir.z*20.0)
                   * smoothstep(0.2, 0.8, dir.y);
        s += vec3(0.85, 0.90, 1.00) * bolt * lflash * 2.5;
        float anvil = fbm(dir * 2.5 + vec3(now*0.015, 0, now*0.008));
        s += vec3(0.08, 0.09, 0.12)
           * smoothstep(0.55, 0.72, anvil) * smoothstep(0.95, 0.5, dir.y) * smoothstep(0.0, 0.3, dir.y) * 0.9;
        s += vec3(0.6, 0.7, 1.0) * lflash * 0.25 * above;
    }
    else if (levelIndex == 3) {
        // Autumn: shooting stars + warm nebula + dense starfield
        for(int k = 0; k < 4; k++) {
            float f = float(k);
            float period = 3.5 + f * 1.3;
            float t2   = fract((now + f * 7.13) / period);
            float seed2 = floor((now + f * 7.13) / period);
            vec3 start = normalize(vec3(hash(vec3(seed2,f,1.0))*2.-1.,
                                        hash(vec3(seed2,f,2.0))*0.5+0.3,
                                        hash(vec3(seed2,f,3.0))*2.-1.));
            vec3 trail = normalize(start + vec3(-0.3,-0.2,0.1));
            float d2 = length(dir - normalize(mix(start, trail, t2)));
            s += vec3(1.0, 0.88, 0.60) * exp(-d2*d2*1800.0) * (1.0-t2) * 2.0 * above;
        }
        s += vec3(0.40, 0.12, 0.05) * smoothstep(0.4, 0.7, fbm(dir*1.5+vec3(0,now*0.004,0))) * above * 0.5;
        s += vec3(1.0, 0.95, 0.80) * starField(dir, 180.0, 55.0) * above * 1.8;
    }
    else if (levelIndex == 4) {
        // Winter: aurora borealis (green/purple/pink) + full moon
        s += vec3(0.1, 1.0, 0.45) * aurora(dir, 0.55, 0.6, 0.0) * 1.2;
        s += vec3(0.45, 0.2, 1.0) * aurora(dir, 0.45, 0.4, 2.71) * 0.8;
        s += vec3(1.0, 0.3, 0.6)  * aurora(dir, 0.65, 0.35, 5.1) * 0.5;
        s += vec3(0.85, 0.90, 1.00) * (starField(dir,220.,11.) + starField(dir,190.,77.)*0.5) * above;
        vec3 md2 = normalize(vec3(-0.5, 0.75, 0.3));
        float md = length(dir - md2);
        s += vec3(0.92, 0.95, 1.00) * smoothstep(0.06, 0.04, md)
           + vec3(0.7, 0.8, 1.0) * exp(-md*md*10.0) * 0.15;
    }
    else if (levelIndex == 5) {
        // Desert sunset: dune silhouettes + dust haze + low red sun
        float dune = sin(dir.x*8.+dir.z*5.+0.5)*0.4 + sin(dir.x*3.-dir.z*7.)*0.6;
        s += vec3(0.22, 0.12, 0.04) * smoothstep(dune*0.08, 0.0, horiz+0.08) * 0.8;
        s += vec3(0.85, 0.55, 0.25)
           * smoothstep(0.4, 0.65, fbm(dir*1.8+vec3(now*0.01,0,0)))
           * smoothstep(0.5, 0.0, horiz) * 0.4;
        float sd = dot(dir, normalize(vec3(0.3, 0.15, -0.8)));
        s += vec3(1.0, 0.35, 0.05) * diskGlow(1.0-sd, 0.001, 0.025) * 1.5;
    }
    else if (levelIndex == 6) {
        // Enchanted forest: bioluminescent ground-glow + firefly sparkles + tendrils
        s += vec3(0.05, 1.0, 0.55) * exp(-horiz*3.) * smoothstep(-0.1,0.3,horiz) * 0.45;
        s += vec3(0.8, 1.0, 0.5) * pow(hash(floor(dir*120.+now*0.15)),28.) * above * 2.0;
        s += vec3(0.3, 0.9, 0.6)
           * smoothstep(0.55, 0.75, fbm(dir*3.+vec3(0,now*.03,now*.02)))
           * smoothstep(0.6, 0.2, horiz) * 0.55;
    }
    else if (levelIndex == 7) {
        // Underwater: caustic light beams + deep blue abyss + rising bubbles
        float c = sin(dir.x*12.+now*.8)*sin(dir.z*9.+now*.6)
                + sin(dir.x*7.-now*.5)*sin(dir.z*15.+now*.4);
        s += vec3(0.1, 0.7, 0.9) * smoothstep(0.5,1.8,c) * smoothstep(0.95,0.4,above) * above * 0.6;
        s += vec3(0.0, 0.05, 0.20) * smoothstep(0.4, 0.0, horiz) * 0.7;
        s += vec3(0.6, 0.9, 1.0) * pow(hash(floor(dir*80.+vec3(0,now*.5,0))),32.) * above * 1.5;
    }
    else if (levelIndex == 8) {
        // Space nebula: purple/blue gas + planetary ring + ultra-dense stars
        s += vec3(0.6, 0.1, 0.9) * smoothstep(0.4,0.7, fbm2(dir*2.+vec3(0,now*.003,0))) * 0.8;
        s += vec3(0.1, 0.5, 1.0) * smoothstep(0.45,0.72,fbm2(dir*2.8+vec3(now*.002,0,.5))) * 0.7;
        s += vec3(1.0, 0.95, 0.85) * (starField(dir,250.,31.) + starField(dir,180.,88.)*0.6) * above * 2.5;
        float ring = abs(dot(dir, normalize(vec3(0.2,1.0,0.3))));
        s += vec3(0.9, 0.75, 0.4) * exp(-ring*ring*400.) * smoothstep(0.,0.5,above) * 0.7;
    }
    else if (levelIndex == 9) {
        // Frozen tundra: huge moon + ice crystal sparkles + Milky Way
        vec3 moonD = normalize(vec3(0.4,0.8,-0.2));
        float md = length(dir - moonD);
        s += vec3(0.90,0.93,1.00) * smoothstep(0.055,0.038,md) * (0.85 + hash(floor(dir*80.))*0.25);
        s += vec3(0.7, 0.8, 1.0) * exp(-md*md*4.) * 0.12;
        s += vec3(0.7, 0.88, 1.0) * pow(hash(floor(dir*300.+now*.05)),26.) * above * 2.0;
        s += vec3(0.65,0.7,0.9) * smoothstep(0.5,0.7, fbm(dir*3.5+vec3(0,.3,0))) * above * 0.35;
    }
    else if (levelIndex == 10) {
        // Volcanic island: lava glow from below + floating ash + ember clouds + crimson sun
        s += vec3(1.0, 0.2, 0.0) * exp(-horiz*2.5) * smoothstep(-0.05,0.2,horiz) * 0.6;
        s += vec3(0.35,0.32,0.28) * pow(hash(floor(dir*150.+now*.1)),25.) * above * 1.2;
        s += vec3(0.9, 0.35, 0.0)
           * smoothstep(0.55,0.72,fbm(dir*2.+vec3(now*.02,0,now*.015)))
           * smoothstep(0.5,0.0,horiz) * 0.5;
        float sd = dot(dir, normalize(vec3(-0.3,0.1,-0.9)));
        s += vec3(1.0, 0.4, 0.0) * diskGlow(1.0-sd, 0.002, 0.04) * 1.8;
    }
    else if (levelIndex == 11) {
        // Celestial garden: floating green islands + root tendrils + golden sky rays + glowing butterflies
        for(int k = 0; k < 4; k++) {
            float f = float(k);
            vec3 idir = normalize(vec3(sin(f*2.3+now*.008), 0.55+f*.07, cos(f*1.7+now*.006)));
            float id = length(dir - idir);
            s += vec3(0.25, 0.52, 0.18) * smoothstep(0.12, 0.06, id);
            float root = exp(-(dir.y-(idir.y-.08))*(dir.y-(idir.y-.08))*80.) * exp(-abs(dir.x-idir.x)*30.) * 0.4;
            s += vec3(0.40, 0.28, 0.12) * root * smoothstep(idir.y, idir.y-0.15, dir.y);
        }
        float ray = pow(sin(dir.x*15.+dir.z*10.+now*.3)*.5+.5, 8.) * smoothstep(0.8,1.0,dir.y) * 0.5;
        s += vec3(1.0, 0.95, 0.60) * ray;
        s += vec3(1.0, 0.8, 0.2) * pow(hash(floor(dir*100.+now*.12)),30.) * above * 2.5;
    }

    return s;
}

void main() {
    vec3 dir    = normalize(TexCoords);
    float horiz = dot(dir, vec3(0, 1, 0));
    float above = clamp(horiz, 0.0, 1.0);
    vec3 col = vec3(0.0);

    if (season == 0) {
        col = mix(vec3(0.58,0.80,1.00), vec3(0.12,0.40,0.88), above);
        col = mix(col, vec3(0.45,0.72,1.00), smoothstep(0.3,0.0,above)*0.4);
    }
    else if (season == 1) {
        col = mix(vec3(0.90,0.88,1.00), vec3(0.32,0.56,0.95), above);
        col = mix(col, vec3(1.00,0.75,0.82), smoothstep(0.35,0.0,above)*0.50);
    }
    else if (season == 2) {
        col = mix(vec3(0.30,0.34,0.38), vec3(0.12,0.15,0.18), above);
        float cloud = fbm(dir * 3.5 + vec3(time*0.05, 0, time*0.03));
        col = mix(col, vec3(0.22,0.25,0.28), smoothstep(0.35,0.72,cloud)*0.75);
        if (horiz > 0.0) {
            float streak = step(0.985, fract((dir.x*100.+dir.z*40.+time*7.)*0.12));
            col = mix(col, vec3(0.45,0.58,0.80), streak*above*0.6);
        }
    }
    else if (season == 3) {
        col = mix(vec3(0.95,0.52,0.15), vec3(0.65,0.28,0.55), smoothstep(0.0,0.35,above));
        col = mix(col, vec3(0.10,0.14,0.48), smoothstep(0.35,1.0,above));
    }
    else {
        col = mix(vec3(0.72,0.80,0.96), vec3(0.30,0.45,0.72), above);
        col = mix(col, vec3(0.90,0.93,0.98), smoothstep(0.52,0.78,fbm(dir*2.5+time*0.018))*0.38);
    }

    if (horiz < 0.0) col = mix(col, vec3(0.08,0.06,0.04), clamp(-horiz*3.5,0.,1.));

    if (season != 2 && horiz > 0.04) {
        float cloud = fbm(dir * 2.0 + time * 0.022);
        float cMask = smoothstep(0.46, 0.63, cloud);
        vec3 cCol = (season==0) ? vec3(1.00,1.00,1.00) :
                    (season==1) ? vec3(1.00,0.92,0.95) :
                    (season==3) ? vec3(0.92,0.72,0.45) : vec3(0.90,0.93,0.98);
        cCol *= 0.78 + smoothstep(0.45,0.65,fbm(dir*2.5+time*0.018+0.5))*0.22;
        col = mix(col, cCol, cMask * smoothstep(0.,0.25,horiz) * smoothstep(0.88,0.38,horiz) * 0.88);
    }

    if (season != 2 && season != 4) {
        float sd = dot(dir, normalize(vec3(0.5,0.6,-0.5)));
        vec3 sunCol = (season==3) ? vec3(1.0,0.65,0.20) : vec3(1.0,0.97,0.82);
        col += sunCol * (smoothstep(0.9990,0.9999,sd)
                       + smoothstep(0.988,0.9990,sd)*0.5
                       + smoothstep(0.96,0.990,sd)*0.2);
    }

    if (season == 4 && horiz > 0.0) {
        col += vec3(0.95,0.97,1.00) * (pow(hash(dir*700.+time*0.18),28.)
                                      + pow(hash(dir*450.+time*0.12),32.)*0.5) * above * 1.6;
    }

    if ((season==3||season==4) && horiz > 0.35) {
        float twinkle = 0.8 + 0.2*sin(time*3.+hash(dir*200.)*40.);
        col += vec3(0.95,0.95,1.00)
             * (pow(hash(dir*520.),42.)*1.4 + pow(hash(dir*360.+11.1),48.)*0.7)
             * (above-0.35) * 2.5 * twinkle;
    }

    col += levelScenery(dir, horiz, above, time);

    FragColor = vec4(clamp(col, 0.0, 1.0), 1.0);
}