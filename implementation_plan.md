# 🎮 Heavy Graphics Upgrade — Implementation Plan

## Overview

Transform the game from simple colored cubes into a visually stunning **sci-fi/arcane perspective puzzle**. All core upgrades are done purely in GLSL + OpenGL (no external assets required). Optional high-quality texture assets are listed below.

---

## What Will Change

### 1. 🌌 Procedural Skybox
A full-screen sky rendered as a large inverted cube drawn before everything else (depth writes disabled).
- Deep space background with **procedural star field** (hash noise in GLSL — thousands of stars)
- Subtle **aurora/nebula color bands** (purple → blue → teal gradient with shimmer)
- Distant **galaxy smear** using layered fbm noise
- No external assets needed — 100% math in the sky fragment shader

### 2. 🪨 Procedural Platform Texturing
Replace flat-color platforms with noise-based surface detail in the fragment shader:
- **Top face**: Voronoi crack pattern (stone tile look) + glowing energy rim around the top edge
- **Side faces**: Layered horizontal bands (ancient stone block look)
- **Glow platforms**: Crystal/energy material — inner blue-white shimmer with pulsing veins
- **Goal platform**: Emerald green with animated runic glow lines

### 3. ✨ Particle System
CPU-managed floating particles rendered as tiny billboarded quads:
- **Ambient dust motes** — slowly drifting upward across the whole scene
- **Platform emitters** — each glowing (illusion-active) platform emits gold ember sparks
- **Goal platform** — green sparkle fountain rising upward
- **Player trail** — when moving, leaves a brief red-orange trail of fading quads

### 4. 🔮 Enhanced Player Character
Replace the red cube player with a **glowing energy orb**:
- Icosphere mesh (smooth sphere) instead of cube
- Animated **pulsing scale** (gentle breathe effect)
- **Inner bright core** + **outer translucent shell** (two-pass: solid core, then additive outer glow)
- When level complete: rapid pulse + color shift to gold

### 5. 🌉 Illusion Connection Beam
When two platforms align (illusion active):
- A **glowing beam of light** connecting their centers — rendered as a thin cylinder/quad strip
- Beam **undulates** using a sine wave along its length
- Color: gold → white → gold, pulsing in sync with the platform glow

### 6. 💡 Platform Edge Glow Strips
Each platform gets 4 thin glowing strips along its top edges:
- Rendered as thin rectangular quads, slightly above the platform top
- Normal platforms: dim blue; Glowing platforms: bright gold; Goal: bright green
- Creates a neon-trim sci-fi aesthetic

### 7. 🪞 Reflective Ground Plane
Replace the grid with a **dark reflective floor**:
- Single large flat quad with a fake reflection (renders platforms again at Y=-pos.y, dimmed)
- Subtle ripple distortion using noise + time uniform

---

## Suggested External Assets (Optional — you get them, I integrate)

| Asset | Source | What it replaces |
|-------|--------|-----------------|
| **Stone/Marble PBR texture** (albedo + normal + roughness) | [ambientcg.com](https://ambientcg.com) → search "Stone" or "Marble" | Platform top face |
| **Sci-fi floor plate texture** | [polyhaven.com](https://polyhaven.com) → Textures → Metal/Sci-fi | Platform sides |
| **Crystal/gem HDR environment** | [polyhaven.com](https://polyhaven.com) → HDRIs | Skybox replacement |
| **Low-poly character model** (OBJ/GLTF) | [sketchfab.com/features/free](https://sketchfab.com/features/free) → filter "Low Poly" | Player cube |
| **Particle/spark texture** (PNG with alpha) | [opengameart.org](https://opengameart.org) → search "particle spark" | Particle quads |

> **Recommended priority:** Download a stone PBR set from ambientcg.com (free, CC0) — these 3 texture maps (albedo, normal, roughness) will make the biggest visual difference with the least complexity.

---

## New File Structure

```
shaders/
  vertex.glsl        ← add UV output + tangent space
  fragment.glsl      ← add noise funcs, procedural texturing, edge glow
  sky_vert.glsl      ← NEW: skybox vertex shader
  sky_frag.glsl      ← NEW: procedural star/nebula sky
  particle_vert.glsl ← NEW: billboard particle vertex shader
  particle_frag.glsl ← NEW: additive glow particle fragment

src/
  main.cpp           ← skybox VAO, particle system, beam, icosphere, edge strips
```

---

## Verification Plan

```bash
cd /home/uchiha/Documents/PROJECTS/CGV
g++ src/main.cpp src/illusion.cpp src/shader.cpp src/glad.c \
    -Iinclude -lglfw -lGL -lm -ldl -std=c++17 -O2 -o game && ./game
```

- [ ] Stars and nebula visible in sky background when orbiting
- [ ] Platforms have stone/crystal texture detail (not flat color)
- [ ] Particles floating around scene; embers on glowing platforms
- [ ] Player is a glowing orb (smooth sphere)
- [ ] Illusion beam appears when platforms align
- [ ] Edge glow strips visible on all platforms
- [ ] Ground reflection visible beneath platforms
- [ ] Solid 60 FPS throughout
