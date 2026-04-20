#!/bin/bash
# Quick Reference for OpenGL Rendering System
# Run: cat OPENGL_QUICK_START.md

# ============================================================================
# OpenGL QUICK START GUIDE
# ============================================================================

# Available Shaders
# ============================================================================
# 1. 'phong'      - Realistic Phong lighting model
# 2. 'toon'       - Stylized cartoon/cel shading with outlines
# 3. 'basic'      - Simple color rendering (fastest)
# 4. 'emission'   - Glowing/self-illuminating materials
# 5. 'normalMap'  - Normal-mapped detailed surfaces
# 6. 'parallax'   - Height-mapped displacement
# 7. 'depth'      - Depth visualization (debug)


# TypeScript Example: Basic Setup
# ============================================================================

import { OpenGLRenderer } from './game/OpenGLRenderer';
import * as THREE from 'three';

// Create renderer
const canvas = document.getElementById('canvas') as HTMLCanvasElement;
const scene = new THREE.Scene();
const camera = new THREE.PerspectiveCamera(75, window.innerWidth / window.innerHeight, 0.1, 1000);

const glRenderer = new OpenGLRenderer(canvas, scene, camera, {
  antialias: true,
  enableDepthTest: true,
  clearColor: new THREE.Color(0x87CEEB)
});

// Add lights
glRenderer.addLight({
  type: 'directional',
  position: new THREE.Vector3(10, 20, 10),
  color: new THREE.Color(1, 1, 1),
  intensity: 1.0
});

glRenderer.addLight({
  type: 'point',
  position: new THREE.Vector3(-5, 5, 5),
  color: new THREE.Color(0.7, 0.7, 1),
  intensity: 0.8
});

// Create and shade objects
const geo = new THREE.SphereGeometry(1, 32, 32);
const mat = new THREE.MeshStandardMaterial({ color: 0xff0000 });
const mesh = new THREE.Mesh(geo, mat);

glRenderer.applyShader(mesh, 'phong');  // Apply shader
scene.add(mesh);

// Start rendering
glRenderer.start(() => {
  // Animation loop
  mesh.rotation.x += 0.01;
  mesh.rotation.y += 0.02;
});


# Common Tasks
# ============================================================================

# Task 1: Apply Phong Lighting (Realistic)
glRenderer.applyShader(mesh, 'phong');
glRenderer.updateShaderUniforms('phong', {
  uSpecular: 0.9,
  uAmbient: 0.4
});

# Task 2: Apply Toon Shading (Stylized)
glRenderer.applyShader(mesh, 'toon');

# Task 3: Make Object Glow
glRenderer.applyShader(mesh, 'emission');
glRenderer.updateShaderUniforms('emission', {
  uEmissionIntensity: 2.5
});

# Task 4: Control Rendering Pipeline
const pipeline = glRenderer.getPipeline();
pipeline.setPipelineState({
  depthTest: true,
  cullFace: true,
  blend: false
});

# Task 5: Monitor Performance
setInterval(() => {
  const info = glRenderer.getInfo();
  console.log('Triangles:', info.render.triangles);
  console.log('Memory:', info.memory);
}, 1000);

# Task 6: Save Screenshot
const screenshot = glRenderer.captureScreenshot();
// screenshot is a data URL that can be downloaded


# Configuration Options
# ============================================================================

interface OpenGLConfig {
  antialias?: boolean;              // Default: true
  pixelRatio?: number;              // Default: window.devicePixelRatio
  enableDepthTest?: boolean;        // Default: true
  enableFaceCulling?: boolean;      // Default: true
  clearColor?: THREE.Color;         // Default: light blue
}


# Light Types
# ============================================================================

// Directional Light (like sun)
glRenderer.addLight({
  type: 'directional',
  position: new THREE.Vector3(10, 20, 10),
  color: new THREE.Color(1, 1, 1),
  intensity: 1.2
});

// Point Light (like bulb)
glRenderer.addLight({
  type: 'point',
  position: new THREE.Vector3(-5, 5, 5),
  color: new THREE.Color(1, 0, 0),
  intensity: 0.8
});

// Spot Light (like flashlight)
glRenderer.addLight({
  type: 'spot',
  position: new THREE.Vector3(0, 10, 0),
  color: new THREE.Color(0, 1, 0),
  intensity: 1.0
});


# API Reference
# ============================================================================

// Shader Management
glRenderer.applyShader(mesh, 'phong');
glRenderer.getShader('phong');
glRenderer.updateShaderUniforms('phong', { /* uniforms */ });

// Rendering Control
glRenderer.start(callback?: () => void);  // Start continuous rendering
glRenderer.stop();                         // Stop rendering
glRenderer.render();                       // Single frame render
glRenderer.isActive(): boolean;            // Check if running

// Lighting
glRenderer.addLight({ /* config */ });
glRenderer.getLights(): GLLight[];

// Pipeline
glRenderer.getPipeline(): GLGraphicsPipeline;
glRenderer.setPipelineState({ /* state */ });
glRenderer.setDepthTest(enabled: boolean);
glRenderer.setFaceCulling(enabled: boolean);

// Utilities
glRenderer.setSize(width: number, height: number);
glRenderer.setClearColor(color: THREE.Color | number);
glRenderer.captureScreenshot(): string;  // Returns data URL
glRenderer.getInfo(): { memory, render };
glRenderer.getCanvas(): HTMLCanvasElement;
glRenderer.getContext(): WebGLRenderingContext;
glRenderer.getRenderTargetTexture(index: number): THREE.Texture | null;
glRenderer.dispose();


# Shader Uniform Examples
# ============================================================================

// Phong Shader Uniforms
{
  uLightPosition: new THREE.Vector3(10, 20, 10),
  uLightColor: new THREE.Color(1, 1, 1),
  uCameraPosition: new THREE.Vector3(0, 0, 10),
  uAmbient: 0.3,      // 0-1
  uSpecular: 0.5      // 0-1
}

// Emission Shader Uniforms
{
  uEmissionIntensity: 2.0  // Can be > 1 for bright glow
}

// Toon Shader Uniforms
{
  uLightPosition: new THREE.Vector3(10, 20, 10),
  uLightColor: new THREE.Color(1, 1, 1),
  uCameraPosition: new THREE.Vector3(0, 0, 10)
}


# Integration with SceneManager
# ============================================================================

// The SceneManager automatically creates and manages OpenGL renderer
const sceneManager = new SceneManager(container, soundManager);

// Access the OpenGL renderer
const glRenderer = sceneManager.glRenderer;

// Use it for custom shading
sceneManager.scene.traverse(object => {
  if (object instanceof THREE.Mesh && object.name === 'block') {
    glRenderer.applyShader(object, 'phong');
  }
});


# Pipeline State Flags
# ============================================================================

interface PipelineState {
  depthTest: boolean;           // Enable/disable depth testing
  depthWrite: boolean;          // Enable/disable depth writes
  cullFace: boolean;            // Enable/disable face culling
  blend: boolean;               // Enable/disable blending
  blendFunc: [number, number];  // [srcFactor, dstFactor]
}

// Common blend functions
THREE.SrcAlphaFactor                // Blend based on source alpha
THREE.OneMinusSrcAlphaFactor       // 1 - source alpha
THREE.SrcColorFactor                // Blend based on source color
THREE.OneMinusSrcColorFactor       // 1 - source color


# WebGL Context Access
# ============================================================================

const context = glRenderer.getContext() as WebGLRenderingContext | WebGL2RenderingContext;

// Check capabilities
const maxLights = context.getParameter(context.MAX_LIGHTS);
const maxTextureSize = context.getParameter(context.MAX_TEXTURE_SIZE);
const vendor = context.getParameter(context.VENDOR);


# Performance Tips
# ============================================================================

1. Choose appropriate shaders:
   - 'basic' for simple objects (fastest)
   - 'toon' for stylized rendering
   - 'phong' for realistic objects
   - 'parallax' only when detail is critical

2. Limit active lights:
   - Use 1-2 directional lights
   - Limit point lights to <5

3. Batch operations:
   - Apply shaders to multiple objects at once
   - Update uniforms before rendering

4. Monitor performance:
   - Use glRenderer.getInfo() to track metrics
   - Watch render calls and triangle count

5. Optimize pipeline state:
   - Only enable features you need
   - Reuse render targets


# Debugging
# ============================================================================

// Check if renderer is running
console.log(glRenderer.isActive());

// Get rendering statistics
const info = glRenderer.getInfo();
console.log('Calls:', info.render.calls);
console.log('Triangles:', info.render.triangles);
console.log('Memory.geometries:', info.memory.geometries);

// Visualize depth
glRenderer.applyShader(mesh, 'depth');

// Get current shader
const shader = glRenderer.getShader('phong');
console.log(shader);

// Get pipeline info
const pipelineState = glRenderer.getPipelineState();
console.log('Current state:', pipelineState);


# Common Issues & Solutions
# ============================================================================

Issue: Objects appear dark
Solution: Add more lights or adjust uAmbient uniform

Issue: Performance drops
Solution: Reduce number of lights or use simpler shaders

Issue: Shaders not applying
Solution: Ensure mesh has proper geometry and material

Issue: Texture not showing
Solution: Check map attribute in shader uniforms


# Resources
# ============================================================================

- Full API docs: See OPENGL_IMPLEMENTATION.md
- Code examples: See src/game/OpenGLExamples.ts
- Shader source: See src/game/GLShaders.ts
- GLSL specs: https://www.khronos.org/files/webgl/webgl-reference-card-1_0.pdf
- Three.js docs: https://threejs.org/docs/

# ============================================================================
